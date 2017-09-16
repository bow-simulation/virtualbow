#pragma once
#include "model/InputData.hpp"
#include "model/output/OutputData.hpp"
#include "model/LimbProperties.hpp"
#include "fem/System.hpp"
#include "fem/Solver.hpp"
#include "fem/elements/BeamElement.hpp"
#include "fem/elements/BarElement.hpp"
#include "fem/elements/MassElement.hpp"
#include "numerics/SecantMethod.hpp"
#include "gui/ProgressDialog.hpp"
#include <algorithm>
#include <json.hpp>

using namespace nlohmann;

class BowModel
{
public:
    template<typename F>
    static OutputData run_static_simulation(const InputData& input, const F& callback)
    {
        BowModel model(input);
        OutputData output;

        model.simulate_setup(output.setup);
        model.simulate_statics(output.statics, callback);

        return output;
    }

    template<typename F1, typename F2>
    static OutputData run_dynamic_simulation(const InputData& input, const F1& callback1, const F2& callback2)
    {
        BowModel model(input);
        OutputData output;

        model.simulate_setup(output.setup);
        model.simulate_statics(output.statics, callback1);
        model.simulate_dynamics(output.dynamics, output.statics, callback2);

        return output;
    }

private:
    BowModel(const InputData& input)
        : input(input)
    {

    }

    // Implementation

    void simulate_setup(SetupData& setup)
    {
        size_t n = input.settings_n_elements_limb;
        size_t k = input.settings_n_elements_string;

        // Create limb nodes
        limb = LimbProperties(input);
        for(size_t i = 0; i < n+1; ++i)
        {
            DofType type = (i == 0) ? DofType::Fixed : DofType::Active;
            Node node = system.create_node({type, type, type}, {limb.x[i], limb.y[i], limb.phi[i]});
            nodes_limb.push_back(node);
        }

        // Create limb elements
        for(size_t i = 0; i < n; ++i)
        {
            double rhoA = 0.5*(limb.rhoA[i] + limb.rhoA[i+1]);
            double L = Node::distance(nodes_limb[i], nodes_limb[i+1]);

            double Cee = 0.5*(limb.Cee[i] + limb.Cee[i+1]);
            double Ckk = 0.5*(limb.Ckk[i] + limb.Ckk[i+1]);
            double Cek = 0.5*(limb.Cek[i] + limb.Cek[i+1]);

            // Todo: Document this
            double phi = Node::angle(nodes_limb[i], nodes_limb[i+1]);
            double phi0 = phi - nodes_limb[i][2].u();
            double phi1 = phi - nodes_limb[i+1][2].u();

            BeamElement element(nodes_limb[i], nodes_limb[i+1], rhoA, L);
            element.set_reference_angles(phi0, phi1);
            element.set_stiffness(Cee, Ckk, Cek);
            system.add_element(element, "limb");
        }

        // Limb tip
        double xt = nodes_limb.back()[0].u();
        double yt = nodes_limb.back()[1].u();

        // String center at brace height
        double xc = 0.0;
        double yc = input.operation_brace_height;

        // Create string nodes
        for(size_t i = 0; i < k; ++i)
        {
            double p = double(i)/double(k);
            double x = xc*(1.0 - p) + xt*p;
            double y = yc*(1.0 - p) + yt*p;

            DofType type_x = (i == 0) ? DofType::Fixed : DofType::Active;
            Node node = system.create_node({type_x, DofType::Active, DofType::Fixed}, {x, y, 0.0});
            nodes_string.push_back(node);
        }
        nodes_string.push_back(nodes_limb.back());

        // Create arrow node
        node_arrow = nodes_string[0];

        // Create string elements
        double EA = input.string_n_strands*input.string_strand_stiffness;
        double rhoA = input.string_n_strands*input.string_strand_density;

        for(size_t i = 0; i < k; ++i)
        {
            BarElement element(nodes_string[i], nodes_string[i+1], 0.0, EA, 0.0, rhoA); // Element lengths are reset later when string length is determined
            system.add_element(element, "string");
        }

        // Create mass elements
        // MassElement(Node nd, double m, double I)
        MassElement mass_limb_tip(nodes_limb.back(), input.mass_limb_tip);
        MassElement mass_string_tip(nodes_string.back(), input.mass_string_tip);
        MassElement mass_string_center(nodes_string.front(), 0.5*input.mass_string_center);   // 0.5 because of symmetric model
        MassElement mass_arrow(node_arrow, 0.5*input.operation_mass_arrow);                   // 0.5 because of symmetric model

        system.add_element(mass_limb_tip, "mass limb tip");
        system.add_element(mass_string_tip, "mass string tip");
        system.add_element(mass_string_center, "mass string center");
        system.add_element(mass_arrow, "mass arrow");

        // Takes a string length, iterates to equilibrium with the constraint of the brace height
        // and returns the angle of the string center
        auto try_string_length = [&](double string_length)
        {
            double L = 0.5*string_length/double(k);
            for(auto& element: system.element_group_mut<BarElement>("string"))
            {
                element.set_length(L);
            }

            StaticSolverDC solver(system, nodes_string[0][1], yc, 1);   // Todo: Reuse solver across function calls?
            solver.step();

            return Node::angle(nodes_string[0], nodes_string[1]);
        };

        // Todo: Perhaps limit the step size of the root finding algorithm to increase robustness.
        double string_length = 2.0*std::hypot(xc - xt, yc - yt);
        string_length = secant_method(try_string_length, 0.95*string_length, 0.9*string_length, 1e-8, 50);   // Todo: Magic numbers

        // Write setup results to output

        setup.limb = limb;
        setup.string_length = string_length;
    }

    template<typename F>
    void simulate_statics(StaticData& statics, const F& callback)
    {
        StaticSolverDC solver(system, nodes_string[0][1], input.operation_draw_length, input.settings_n_draw_steps);
        while(solver.step())
        {
            add_bow_state(statics.states);

            int progress = (nodes_string[0][1].u() - input.operation_brace_height)/(input.operation_draw_length - input.operation_brace_height)*100.0;

            if(!callback(progress))
                return;
        }

        // Calculate scalar values

        double draw_length_front = statics.states.draw_length.front();
        double draw_length_back = statics.states.draw_length.back();
        double draw_force_back = statics.states.draw_force.back();

        double e_pot_front = statics.states.e_pot_limbs.front()
                           + statics.states.e_pot_string.front();

        double e_pot_back = statics.states.e_pot_limbs.back()
                          + statics.states.e_pot_string.back();

        statics.final_draw_force = draw_force_back;
        statics.drawing_work     = e_pot_back - e_pot_front;
        statics.storage_ratio    = (e_pot_back - e_pot_front)/(0.5*(draw_length_back - draw_length_front)*draw_force_back);
    }

    template<typename F>
    void simulate_dynamics(DynamicData& dynamics, const StaticData& statics, const F& callback)
    {
        // Set draw force to zero    // Todo: Doesn't really belong in this method
        nodes_string[0][1].p_mut() = 0.0;

        double T = std::numeric_limits<double>::max();
        double alpha = input.settings_time_span_factor;     // Todo: Magic number // Todo: Make this a setting

        DynamicSolver solver1(system, input.settings_time_step_factor, input.settings_sampling_rate, [&]
        {
            double ut = node_arrow[1].u();
            if(ut >= input.operation_brace_height)
            {
                double u0 = input.operation_draw_length;
                double u1 = input.operation_brace_height;

                if(ut != u0)
                {
                    T = system.t()*std::acos(u1/u0)/std::acos(ut/u0);    // Cosine approximation
                    //T = system.get_time()*std::sqrt((u1 - u0)/(ut - u0));    // Quadratic approximation
                }
            }

            return node_arrow[1].a() >= 0;
            //return system.t() >= alpha*T;
        });

        auto run_solver = [&](DynamicSolver& solver)
        {
            while(solver.step())
            {
                add_bow_state(dynamics.states);
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

        dynamics.final_arrow_velocity = std::abs(dynamics.states.vel_arrow.back());
        dynamics.final_arrow_energy = std::abs(dynamics.states.e_kin_arrow.back());
        dynamics.efficiency = dynamics.final_arrow_energy/statics.drawing_work;
    }

    void add_bow_state(BowStates& states) const
    {
        states.time.push_back(system.t());
        states.draw_force.push_back(2.0*nodes_string[0][1].p());
        states.draw_length.push_back(nodes_string[0][1].u());

        states.pos_arrow.push_back(input.operation_draw_length - node_arrow[1].u());
        states.vel_arrow.push_back(-node_arrow[1].v());
        states.acc_arrow.push_back(-node_arrow[1].a());

        states.e_pot_limbs.push_back(2.0*system.get_potential_energy("limb", "mass limb tip"));
        states.e_kin_limbs.push_back(2.0*system.get_kinetic_energy("limb", "mass limb tip"));
        states.e_pot_string.push_back(2.0*system.get_potential_energy("string", "mass string tip", "mass string center"));
        states.e_kin_string.push_back(2.0*system.get_kinetic_energy("string", "mass string tip", "mass string center"));
        states.e_kin_arrow.push_back(2.0*system.get_kinetic_energy("mass arrow"));

        // Arrow, limb and string coordinates

        states.y_arrow.push_back(node_arrow[1].u());
        states.x_limb.push_back(std::valarray<double>(nodes_limb.size()));
        states.y_limb.push_back(std::valarray<double>(nodes_limb.size()));

        for(size_t i = 0; i < nodes_limb.size(); ++i)
        {
            states.x_limb.back()[i] = nodes_limb[i][0].u();
            states.y_limb.back()[i] = nodes_limb[i][1].u();
        }

        states.x_string.push_back(std::valarray<double>(nodes_string.size()));
        states.y_string.push_back(std::valarray<double>(nodes_string.size()));

        for(size_t i = 0; i < nodes_string.size(); ++i)
        {
            states.x_string.back()[i] = nodes_string[i][0].u();
            states.y_string.back()[i] = nodes_string[i][1].u();
        }

        // Stresses

        std::valarray<double> epsilon(nodes_limb.size());
        std::valarray<double> kappa(nodes_limb.size());

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

        states.sigma_back.push_back(limb.layers[0].sigma_back(epsilon, kappa));
        states.sigma_belly.push_back(limb.layers[0].sigma_belly(epsilon, kappa));
    }

private:
    const InputData& input;
    LimbProperties limb;

    System system;
    std::vector<Node> nodes_limb;
    std::vector<Node> nodes_string;
    Node node_arrow;
};
