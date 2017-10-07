#pragma once
#include "model/InputData.hpp"
#include "model/output/OutputData.hpp"
#include "model/LimbProperties.hpp"
#include "fem/System.hpp"
#include "fem/StaticSolver.hpp"
#include "fem/DynamicSolver.hpp"
#include "fem/elements/BeamElement.hpp"
#include "fem/elements/BarElement.hpp"
#include "fem/elements/MassElement.hpp"
#include "fem/elements/ConstraintElement.hpp"
#include "fem/elements/ContactSurface.hpp"
#include "numerics/RootFinding.hpp"
#include "numerics/Geometry.hpp"
#include "gui/ProgressDialog.hpp"
#include <algorithm>

class BowModel
{
public:
    template<typename F>
    static OutputData run_static_simulation(const InputData& input, const F& callback)
    {
        BowModel model(input);
        model.simulate_statics(callback);

        return model.output;
    }

    template<typename F1, typename F2>
    static OutputData run_dynamic_simulation(const InputData& input, const F1& callback1, const F2& callback2)
    {
        BowModel model(input);
        model.simulate_statics(callback1);
        model.simulate_dynamics(callback2);

        return model.output;
    }

private:
    BowModel(const InputData& input)
        : input(input)
    {
        init_limb();
        qInfo() << "Limb setup successful!";
        init_string();
        qInfo() << "String setup successful!";
        init_masses();
    }

    // Implementation

    void init_limb()
    {
        // Calculate discrete limb properties
        output.setup.limb = LimbProperties(input);

        // Create limb nodes
        for(size_t i = 0; i < input.settings_n_elements_limb + 1; ++i)
        {
            DofType type = (i == 0) ? DofType::Fixed : DofType::Active;
            Node node = system.create_node({type, type, type}, {output.setup.limb.x[i], output.setup.limb.y[i], output.setup.limb.phi[i]});
            nodes_limb.push_back(node);
        }

        // Create limb elements
        for(size_t i = 0; i < input.settings_n_elements_limb; ++i)
        {
            double rhoA = 0.5*(output.setup.limb.rhoA[i] + output.setup.limb.rhoA[i+1]);
            double L = Node::distance(nodes_limb[i], nodes_limb[i+1]);

            double Cee = 0.5*(output.setup.limb.Cee[i] + output.setup.limb.Cee[i+1]);
            double Ckk = 0.5*(output.setup.limb.Ckk[i] + output.setup.limb.Ckk[i+1]);
            double Cek = 0.5*(output.setup.limb.Cek[i] + output.setup.limb.Cek[i+1]);

            // Todo: Document this
            double phi = Node::angle(nodes_limb[i], nodes_limb[i+1]);
            double phi0 = phi - nodes_limb[i][2].u();
            double phi1 = phi - nodes_limb[i+1][2].u();

            BeamElement element(nodes_limb[i], nodes_limb[i+1], rhoA, L);
            element.set_reference_angles(phi0, phi1);
            element.set_stiffness(Cee, Ckk, Cek);
            system.add_element(element, "limb");
        }

        // Function that applies a torque to the limb tip and returns the difference
        // between limb belly and brace height in the direction of draw
        StaticSolverLC solver(system);
        auto try_torque = [&](double torque)
        {
            // Apply torque, iterate to equilibrium, remove torque again
            nodes_limb.back()[2].p_mut() = torque;
            solver.solve();
            nodes_limb.back()[2].p_mut() = 0.0;

            // Calculate point on the limb that is closest to brace height
            double y_min = std::numeric_limits<double>::max();
            for(size_t i = 0; i < nodes_limb.size(); ++i)
                y_min = std::min(y_min, nodes_limb[i][1].u() - output.setup.limb.h[i]*cos(nodes_limb[i][2].u()));

            return y_min + input.operation_brace_height;    // Todo: Use dimensionless measure
        };

        // Apply a torque to the limb such that the difference to brace height is zero
        secant_method(try_torque, -1.0, -10.0, 1e-5, 50);
    }

    void init_string()
    {
        std::vector<Vector<2>> points;
        points.push_back({0.0, -input.operation_brace_height});
        for(size_t i = 0; i < nodes_limb.size(); ++i)
        {
            points.push_back({
                nodes_limb[i][0].u() + output.setup.limb.h[i]*sin(nodes_limb[i][2].u()),
                nodes_limb[i][1].u() - output.setup.limb.h[i]*cos(nodes_limb[i][2].u())
            });
        }

        points = constant_orientation_subset(points, true);
        points = equipartition(points, input.settings_n_elements_string + 1);

        // Create string nodes
        for(size_t i = 0; i < points.size(); ++i)
        {
            std::array<DofType, 3> dof_types = {
                (i != 0) ? DofType::Active : DofType::Fixed,
                           DofType::Active,
                (i != 0) ? DofType::Active : DofType::Fixed
            };

            Node node = system.create_node(dof_types, {points[i][0], points[i][1], 0.0});
            nodes_string.push_back(node);
        }

        // Create string elements
        double EA = input.string_n_strands*input.string_strand_stiffness;
        double rhoA = input.string_n_strands*input.string_strand_density;

        for(size_t i = 0; i < input.settings_n_elements_string; ++i)
        {
            BeamElement element(nodes_string[i], nodes_string[i+1], rhoA, 0.0);    // Length is set later when determining string length
            element.set_stiffness(EA, 1e-8, 0.0);
            system.add_element(element, "string");
        }

        // Create limb tip constraint and string to limb contact surface
        //double k = 100.0*EA/output.setup.string_length;
        double k = output.setup.limb.Cee[0]/(output.setup.limb.s[1] - output.setup.limb.s[0]);    // Stiffness estimate based on limb data
        system.add_element(ConstraintElement(nodes_limb.back(), nodes_string.back(), k), "constraint");
        system.add_element(ContactSurface(nodes_limb, nodes_string, output.setup.limb.h, k), "contact");

        // Function that sets the sting element length and returns the
        // resulting difference between actual and desired brace height
        StaticSolverLC solver(system);
        auto try_element_length = [&](double l)
        {
            for(auto& element: system.element_group_mut<BeamElement>("string"))
                element.set_length(l);

            qInfo() << "l = " << l;

            solver.solve();
            return nodes_string[0][1].u() + input.operation_brace_height;    // Todo: Use dimensionless measure
        };

        // Find a element length at which the brace height difference is zero
        // Todo: Perhaps limit the step size of the root finding algorithm to increase robustness.
        double l = (points[1] - points[0]).norm();
        l = secant_method(try_element_length, 0.99*l, 0.98*l, 1e-6, 50);

        //try_element_length(0.99*l);

        // Assign setup data
        output.setup.string_length = 2.0*l*input.settings_n_elements_string;    // *2 because of symmetry
    }

    void init_masses()
    {
        node_arrow = nodes_string[0];
        MassElement mass_limb_tip(nodes_limb.back(), input.mass_limb_tip);
        MassElement mass_string_tip(nodes_string.back(), input.mass_string_tip);
        MassElement mass_string_center(nodes_string.front(), 0.5*input.mass_string_center);   // 0.5 because of symmetry
        MassElement mass_arrow(node_arrow, 0.5*input.operation_mass_arrow);                   // 0.5 because of symmetry

        system.add_element(mass_limb_tip, "mass limb tip");
        system.add_element(mass_string_tip, "mass string tip");
        system.add_element(mass_string_center, "mass string center");
        system.add_element(mass_arrow, "mass arrow");
    }

    template<typename F>
    void simulate_statics(const F& callback)
    {
        nodes_string[0][1].p_mut() = 1.0;    // Will be scaled by the static algorithm
        StaticSolverDC solver(system, nodes_string[0][1]);
        for(unsigned i = 0; i < input.settings_n_draw_steps; ++i)
        {
            double eta = double(i)/(input.settings_n_draw_steps - 1);
            double draw_length = (1.0 - eta)*input.operation_brace_height + eta*input.operation_draw_length;

            solver.solve(-draw_length);
            add_bow_state(output.statics.states);

            if(!callback(100*eta))
                return;
        }

        // Calculate scalar values

        double draw_length_front = output.statics.states.draw_length.front();
        double draw_length_back = output.statics.states.draw_length.back();
        double draw_force_back = output.statics.states.draw_force.back();

        double e_pot_front = output.statics.states.e_pot_limbs.front()
                           + output.statics.states.e_pot_string.front();

        double e_pot_back = output.statics.states.e_pot_limbs.back()
                          + output.statics.states.e_pot_string.back();

        output.statics.final_draw_force = draw_force_back;
        output.statics.drawing_work = e_pot_back - e_pot_front;
        output.statics.storage_ratio = (e_pot_back - e_pot_front)/(0.5*(draw_length_back - draw_length_front)*draw_force_back);
    }

    template<typename F>
    void simulate_dynamics(const F& callback)
    {
        // Set draw force to zero
        nodes_string[0][1].p_mut() = 0.0;

        double T = std::numeric_limits<double>::max();
        double alpha = input.settings_time_span_factor;

        DynamicSolver solver1(system, input.settings_time_step_factor, input.settings_sampling_rate, [&]
        {
            double ut = node_arrow[1].u();
            if(ut < input.operation_brace_height)
            {
                double u0 = -input.operation_draw_length;
                double u1 = -input.operation_brace_height;

                if(ut != u0)
                    T = system.t()*std::acos(u1/u0)/std::acos(ut/u0);
            }

            return node_arrow[1].a() < 0;
        });

        auto run_solver = [&](DynamicSolver& solver)
        {
            while(solver.step())
            {
                add_bow_state(output.dynamics.states);
                if(!callback(100.0*system.t()/(alpha*T)))
                    return;
            }
        };

        run_solver(solver1);

        // Change model by giving the arrow an independent node with the initial position and velocity of the string center
        // Todo: Would more elegant to remove the mass element from the system and create a new one with a new node.
        node_arrow = system.create_node(nodes_string[0], {DofType::Fixed, DofType::Active, DofType::Fixed});
        system.element_mut<MassElement>("mass arrow").set_node(node_arrow);

        DynamicSolver solver2(system, input.settings_time_step_factor, input.settings_sampling_rate, [&]
        {
            return system.t() >= alpha*T;
        });

        run_solver(solver2);

        // Calculate scalar values

        output.dynamics.final_arrow_velocity = output.dynamics.states.vel_arrow.back();
        output.dynamics.final_arrow_energy = output.dynamics.states.e_kin_arrow.back();
        output.dynamics.efficiency = output.dynamics.final_arrow_energy/output.statics.drawing_work;
    }

    void add_bow_state(BowStates& states) const
    {
        states.time.push_back(system.t());
        states.draw_force.push_back(-2.0*nodes_string[0][1].p());    // *2 because of symmetry
        states.draw_length.push_back(-nodes_string[0][1].u());

        states.pos_arrow.push_back(node_arrow[1].u());
        states.vel_arrow.push_back(node_arrow[1].v());
        states.acc_arrow.push_back(node_arrow[1].a());

        // *2 because of symmetry
        states.e_pot_limbs.push_back(2.0*system.get_potential_energy("limb", "mass limb tip"));
        states.e_kin_limbs.push_back(2.0*system.get_kinetic_energy("limb", "mass limb tip"));
        states.e_pot_string.push_back(2.0*system.get_potential_energy("string", "mass string tip", "mass string center"));
        states.e_kin_string.push_back(2.0*system.get_kinetic_energy("string", "mass string tip", "mass string center", "contact", "constraint"));
        states.e_kin_arrow.push_back(2.0*system.get_kinetic_energy("mass arrow"));

        // Limb and string coordinates
        states.x_limb.push_back(VectorXd(nodes_limb.size()));
        states.y_limb.push_back(VectorXd(nodes_limb.size()));

        for(size_t i = 0; i < nodes_limb.size(); ++i)
        {
            states.x_limb.back()[i] = nodes_limb[i][0].u();
            states.y_limb.back()[i] = nodes_limb[i][1].u();
        }

        states.x_string.push_back(VectorXd(nodes_string.size()));
        states.y_string.push_back(VectorXd(nodes_string.size()));

        for(size_t i = 0; i < nodes_string.size(); ++i)
        {
            states.x_string.back()[i] = nodes_string[i][0].u();
            states.y_string.back()[i] = nodes_string[i][1].u();
        }

        // Stresses

        VectorXd epsilon(nodes_limb.size());
        VectorXd kappa(nodes_limb.size());

        auto elements = system.element_group<BeamElement>("limb");
        for(size_t i = 0; i < nodes_limb.size(); ++i)
        {
            if(i == 0)
            {
                epsilon[i] = elements[i].get_epsilon();
                kappa[i] = elements[i].get_kappa(0.0);
            }
            else if(i == nodes_limb.size()-1)
            {
                epsilon[i] = elements[i-1].get_epsilon();
                kappa[i] = elements[i-1].get_kappa(1.0);
            }
            else
            {
                epsilon[i] = 0.5*(elements[i-1].get_epsilon() + elements[i].get_epsilon());
                kappa[i] = 0.5*(elements[i-1].get_kappa(1.0) + elements[i].get_kappa(0.0));
            }
        }

        states.sigma_back.push_back(output.setup.limb.layers[0].sigma_back(epsilon, kappa));
        states.sigma_belly.push_back(output.setup.limb.layers[0].sigma_belly(epsilon, kappa));
    }

private:
    const InputData& input;
    OutputData output;

    System system;
    std::vector<Node> nodes_limb;
    std::vector<Node> nodes_string;
    Node node_arrow;
};
