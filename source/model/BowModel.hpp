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
#include <boost/range/algorithm.hpp>
#include <algorithm>
#include <json.hpp>

using namespace nlohmann;

class BowModel
{
public:
    static OutputData run_setup_simulation(const InputData& input)
    {
        BowModel model(input);

        OutputData output;
        model.simulate_setup(output.setup);

        return output;
    }

    template<class F>
    static OutputData run_static_simulation(const InputData& input, const F& callback)
    {
        BowModel model(input);

        OutputData output;
        model.simulate_setup(output.setup);
        model.simulate_statics(output.statics, callback);

        return output;
    }

    template<class F1, class F2>
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
            bool active = (i != 0);
            Node node = system.create_node({active, active, active}, {limb.x[i], limb.y[i], limb.phi[i]});
            nodes_limb.push_back(node);
        }

        // Create limb elements
        for(size_t i = 0; i < n; ++i)
        {
            double rhoA = 0.5*(limb.rhoA[i] + limb.rhoA[i+1]);
            double L = system.get_distance(nodes_limb[i], nodes_limb[i+1]);

            double Cee = 0.5*(limb.Cee[i] + limb.Cee[i+1]);
            double Ckk = 0.5*(limb.Ckk[i] + limb.Ckk[i+1]);
            double Cek = 0.5*(limb.Cek[i] + limb.Cek[i+1]);

            // Todo: Document this
            double phi = system.get_angle(nodes_limb[i], nodes_limb[i+1]);
            double phi0 = phi - system.get_u(nodes_limb[i].phi);
            double phi1 = phi - system.get_u(nodes_limb[i+1].phi);

            BeamElement element(system, nodes_limb[i], nodes_limb[i+1], rhoA, L);
            element.set_reference_angles(phi0, phi1);
            element.set_stiffness(Cee, Ckk, Cek);
            system.mut_elements().push_back(element, "limb");
        }

        // Limb tip
        double xt = system.get_u(nodes_limb.back().x);
        double yt = system.get_u(nodes_limb.back().y);

        // String center at brace height
        double xc = 0.0;
        double yc = input.operation_brace_height;

        // Create string nodes
        for(size_t i = 0; i < k; ++i)
        {
            double p = double(i)/double(k);
            double x = xc*(1.0 - p) + xt*p;
            double y = yc*(1.0 - p) + yt*p;

            bool active_x = (i != 0);
            Node node = system.create_node({active_x, true, false}, {x, y, 0.0});
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
            BarElement element(system, nodes_string[i], nodes_string[i+1], 0.0, EA, 0.0, rhoA); // Element lengths are reset later when string length is determined
            system.mut_elements().push_back(element, "string");
        }

        // Create mass elements
        // MassElement(Node nd, double m, double I)
        MassElement mass_limb_tip(system, nodes_limb.back(), input.mass_limb_tip);
        MassElement mass_string_tip(system, nodes_string.back(), input.mass_string_tip);
        MassElement mass_string_center(system, nodes_string.front(), 0.5*input.mass_string_center);   // 0.5 because of symmetric model
        MassElement mass_arrow(system, node_arrow, 0.5*input.operation_mass_arrow);                   // 0.5 because of symmetric model

        system.mut_elements().push_back(mass_limb_tip, "limb tip");
        system.mut_elements().push_back(mass_string_tip, "string tip");
        system.mut_elements().push_back(mass_string_center, "string center");
        system.mut_elements().push_back(mass_arrow, "arrow");

        // Takes a string length, iterates to equilibrium with the constraint of the brace height
        // and returns the angle of the string center
        auto try_string_length = [&](double string_length)
        {
            double L = 0.5*string_length/double(k);
            for(auto& element: system.mut_elements().group<BarElement>("string"))
            {
                element.set_length(L);
            }

            StaticSolverDC solver(system, nodes_string[0].y, yc, 1);   // Todo: Reuse solver across function calls?
            solver.step();

            return system.get_angle(nodes_string[0], nodes_string[1]);
        };

        // Todo: Perhaps limit the step size of the root finding algorithm to increase robustness.
        double string_length = 2.0*std::hypot(xc - xt, yc - yt);
        string_length = secant_method(try_string_length, 0.95*string_length, 0.9*string_length, 1e-8, 50);   // Todo: Magic numbers

        // Write setup results to output

        setup.limb = limb;
        setup.string_length = string_length;
    }

    template<class F>
    void simulate_statics(StaticData& statics, const F& callback)
    {
        StaticSolverDC solver(system, nodes_string[0].y, input.operation_draw_length, input.settings_n_draw_steps);
        while(solver.step())
        {
            add_bow_state(statics.states);

            int progress = (system.get_u(nodes_string[0].y) - input.operation_brace_height)/(input.operation_draw_length - input.operation_brace_height)*100.0;

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

    template<class F>
    void simulate_dynamics(DynamicData& dynamics, const StaticData& statics, const F& callback)
    {
        // Set draw force to zero    // Todo: Doesn't really belong in this method
        //system.set_p(nodes_string[0].y, 0.0);

        double T = std::numeric_limits<double>::max();
        double alpha = input.settings_time_span_factor;     // Todo: Magic number // Todo: Make this a setting

        DynamicSolver solver1(system, input.settings_time_step_factor, input.settings_sampling_rate, [&]
        {
            double ut = system.get_u(node_arrow.y);
            if(ut >= input.operation_brace_height)
            {
                double u0 = input.operation_draw_length;
                double u1 = input.operation_brace_height;

                if(ut != u0)
                {
                    T = system.get_t()*std::acos(u1/u0)/std::acos(ut/u0);    // Cosine approximation
                    //T = system.get_time()*std::sqrt((u1 - u0)/(ut - u0));    // Quadratic approximation
                }
            }

            return system.get_a(node_arrow.y) >= 0;
            //return system.get_t() >= alpha*T;
        });

        auto run_solver = [&](DynamicSolver& solver)
        {
            while(solver.step())
            {
                add_bow_state(dynamics.states);
                if(!callback(100.0*system.get_t()/(alpha*T)))
                    return;
            }
        };

        run_solver(solver1);

        // Change model by giving the arrow an independent node with the initial position and velocity of the string center
        // Todo: Would more elegant to remove the mass element from the system and create a new one with a new node.
        // Or initially add arrow mass to string center, then subtract that and give arrow its own node and element.
        node_arrow = system.create_node(nodes_string[0]);

        system.mut_elements().front<MassElement>("arrow").set_node(node_arrow);

        DynamicSolver solver2(system, input.settings_time_step_factor, input.settings_sampling_rate, [&]
        {
            return system.get_t() >= alpha*T;
        });

        run_solver(solver2);

        // Calculate scalar values

        dynamics.final_arrow_velocity = std::abs(dynamics.states.vel_arrow.back());
        dynamics.final_arrow_energy = std::abs(dynamics.states.e_kin_arrow.back());
        dynamics.efficiency = dynamics.final_arrow_energy/statics.drawing_work;
    }

    void add_bow_state(BowStates& states) const
    {
        states.time.push_back(system.get_t());
        states.draw_length.push_back(system.get_u(nodes_string[0].y));

        states.draw_force.push_back(2.0*system.get_p(nodes_string[0].y));    // *2 because of symmetry

        double string_force = 0.0;
        for(auto& element: system.get_elements().group<BarElement>("string"))
            string_force = std::max(string_force, std::abs(element.get_normal_force()));

        states.string_force.push_back(string_force);
        states.strand_force.push_back(string_force/input.string_n_strands);
        states.grip_force.push_back(-2.0*system.get_q(nodes_limb[0].y));    // *2 because of symmetry

        states.pos_arrow.push_back(input.operation_draw_length - system.get_u(node_arrow.y));
        states.vel_arrow.push_back(-system.get_v(node_arrow.y));
        states.acc_arrow.push_back(-system.get_a(node_arrow.y));

        double e_pot_limb = 2.0*system.get_elements().get_potential_energy("limb");
        double e_kin_limb = 2.0*system.get_elements().get_kinetic_energy("limb");

        double e_pot_limb_tip = 2.0*system.get_elements().get_potential_energy("limb tip");
        double e_kin_limb_tip = 2.0*system.get_elements().get_kinetic_energy("limb tip");

        double e_pot_string = 2.0*system.get_elements().get_potential_energy("string");
        double e_kin_string = 2.0*system.get_elements().get_kinetic_energy("string");

        double e_pot_string_tip = 2.0*system.get_elements().get_potential_energy("string tip");
        double e_kin_string_tip = 2.0*system.get_elements().get_kinetic_energy("string tip");

        double e_pot_string_center = 2.0*system.get_elements().get_potential_energy("string center");
        double e_kin_string_center = 2.0*system.get_elements().get_kinetic_energy("string center");

        double e_kin_arrow = 2.0*system.get_elements().get_kinetic_energy("arrow");

        states.e_pot_limbs.push_back(e_pot_limb + e_pot_limb_tip);
        states.e_kin_limbs.push_back(e_kin_limb + e_kin_limb_tip);
        states.e_pot_string.push_back(e_pot_string + e_pot_string_tip + e_pot_string_center);
        states.e_kin_string.push_back(e_kin_string + e_kin_string_tip + e_kin_string_center);
        states.e_kin_arrow.push_back(e_kin_arrow);

        // Arrow, limb and string coordinates

        states.y_arrow.push_back(system.get_u(node_arrow.y));
        states.x_limb.push_back(std::valarray<double>(nodes_limb.size()));
        states.y_limb.push_back(std::valarray<double>(nodes_limb.size()));

        for(size_t i = 0; i < nodes_limb.size(); ++i)
        {
            states.x_limb.back()[i] = system.get_u(nodes_limb[i].x);
            states.y_limb.back()[i] = system.get_u(nodes_limb[i].y);
        }

        states.x_string.push_back(std::valarray<double>(nodes_string.size()));
        states.y_string.push_back(std::valarray<double>(nodes_string.size()));

        for(size_t i = 0; i < nodes_string.size(); ++i)
        {
            states.x_string.back()[i] = system.get_u(nodes_string[i].x);
            states.y_string.back()[i] = system.get_u(nodes_string[i].y);
        }

        // Stresses

        // Todo: Keep those allocated somewhere
        std::valarray<double> epsilon(nodes_limb.size());
        std::valarray<double> kappa(nodes_limb.size());

        auto elements = system.get_elements().group<BeamElement>("limb");
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
