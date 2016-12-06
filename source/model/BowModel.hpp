#pragma once
#include "InputData.hpp"
#include "OutputData.hpp"

#include "DiscreteLimb.hpp"
#include "../fem/System.hpp"
#include "../fem/elements/BeamElement.hpp"
#include "../fem/elements/BarElement.hpp"
#include "../fem/elements/MassElement.hpp"
#include "../fem/elements/ContactElement1D.hpp"
#include "../numerics/SecantMethod.hpp"
#include "../gui/ProgressDialog.hpp"
#include <boost/optional.hpp>
#include <algorithm>

class BowModel
{
public:
    BowModel(const InputData& input, OutputData& output)
        : input(input), output(output)
    {

    }

    void simulate_setup()
    {
        size_t n = input.settings_n_elements_limb;
        size_t k = input.settings_n_elements_string;

        // Create limb nodes
        limb = DiscreteLimb(input);
        for(size_t i = 0; i < n+1; ++i)
        {
            bool active = (i != 0);
            Node node = system.create_node({{active, active, active}}, {{limb.x[i], limb.y[i], limb.phi[i]}});
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

            BeamElement element(nodes_limb[i], nodes_limb[i+1], rhoA, L);
            element.set_reference_angles(phi0, phi1);
            element.set_stiffness(Cee, Ckk, Cek);
            system.elements().add(element, "limb");
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

            Node node = system.create_node({{(i != 0), true, false}}, {{x, y, 0.0}});
            nodes_string.push_back(node);
        }
        nodes_string.push_back(nodes_limb.back());

        // Create arrow node
        node_arrow = nodes_string[0];   //system.create_node({{xc, yc, 0.0}}, {{false, true, false}});

        // Create string elements
        double EA = input.string_n_strands*input.string_strand_stiffness;
        double rhoA = input.string_n_strands*input.string_strand_density;

        for(size_t i = 0; i < k; ++i)
        {
            BarElement element(nodes_string[i], nodes_string[i+1], 0.0, EA, 0.0, rhoA); // Element lengths are reset later when string length is determined
            system.elements().add(element, "string");
        }

        // Create mass elements
        // MassElement(Node nd, double m, double I)
        MassElement mass_limb_tip(nodes_limb.back(), input.mass_limb_tip);
        MassElement mass_string_tip(nodes_string.back(), input.mass_string_tip);
        MassElement mass_string_center(nodes_string.front(), 0.5*input.mass_string_center);   // 0.5 because of symmetric model
        MassElement mass_arrow(node_arrow, 0.5*input.operation_mass_arrow);                   // 0.5 because of symmetric model

        system.elements().add(mass_limb_tip, "mass limb tip");
        system.elements().add(mass_string_tip, "mass string tip");
        system.elements().add(mass_string_center, "mass string center");
        system.elements().add(mass_arrow, "mass arrow");

        // Takes a string length, iterates to equilibrium with the constraint of the brace height
        // and returns the angle of the string center
        auto try_string_length = [&](double string_length)
        {
            double L = 0.5*string_length/double(k);
            for(auto& element: system.elements_const().group<BarElement>("string"))
            {
                element.set_length(L);
            }

            Dof dof = nodes_string[0].y;
            system.solve_statics_dc(dof, yc, 1);

            return system.get_angle(nodes_string[0], nodes_string[1]);
        };

        // Todo: Perhaps limit the step size of the root finding algorithm to increase robustness.
        double string_length = 2.0*std::hypot(xc - xt, yc - yt);
        string_length = secant_method(try_string_length, 0.95*string_length, 0.9*string_length, 1e-8, 50);   // Todo: Magic numbers

        // Store setup results
        output.setup.limb = limb;
        output.setup.string_length = string_length;
    }

    void simulate_statics(TaskState& task)
    {
        BowStates states;
        system.solve_statics_dc(nodes_string[0].y, input.operation_draw_length, input.settings_n_draw_steps, [&]()   // Todo: Magic number
        {
            get_bow_state(states);
            task.setProgress((system.get_u(nodes_string[0].y) - input.operation_brace_height)/
                             (input.operation_draw_length - input.operation_brace_height)*100.0);

            return !task.isCanceled();
        });

        output.statics = std::make_unique<StaticData>(states);
    }

    void simulate_dynamics(TaskState& task)
    {
        // Remove draw force    // Todo: Doesn't really belong in this method
        system.get_p(nodes_string[0].y) = 0.0;

        double T = 0.0;
        double alpha = input.settings_time_span_factor;     // Todo: Magic number // Todo: Make this a setting

        BowStates states;
        auto sample = [&]()
        {
            get_bow_state(states);

            // If brace height was not yet reached update estimate T
            double u = system.get_u(node_arrow.y);
            if(u >= input.operation_brace_height)
            {
                double u0 = input.operation_draw_length;
                double u1 = input.operation_brace_height;

                if(u == u0)
                    return true;

                //T = system.get_time()*std::sqrt((u1 - u0)/(u - u0));    // Quadratic approximation
                T = system.get_time()*std::acos(u1/u0)/std::acos(u/u0);    // Cosine approximation
            }

            task.setProgress(100.0*system.get_time()/(alpha*T));
            return system.get_time() < alpha*T;
        };

        double t = 0.0;
        double dt = input.settings_sampling_time;
        system.solve_dynamics(input.settings_time_step_factor, [&]()
        {
            if(system.get_time() >= t)
            {
                t += dt;
                return sample() && !task.isCanceled();
            }

            return !task.isCanceled();
        });

        output.dynamics = std::make_unique<DynamicData>(states, *output.statics);
    }

    void get_bow_state(BowStates& states) const
    {
        states.time.push_back(system.get_time());
        states.draw_force.push_back(2.0*system.get_p(nodes_string[0].y));   // *2.0 because symmetry

        states.pos_arrow.push_back(system.get_u(node_arrow.y));
        states.vel_arrow.push_back(system.get_v(node_arrow.y));
        states.acc_arrow.push_back(system.get_a(node_arrow.y));

        states.pos_string.push_back(system.get_u(nodes_string[0].y));
        states.vel_string.push_back(system.get_v(nodes_string[0].y));
        states.acc_string.push_back(system.get_a(nodes_string[0].y));

        states.e_pot_limbs.push_back(2.0*(system.get_potential_energy("limb") + system.get_potential_energy("mass limb tip")));
        states.e_kin_limbs.push_back(2.0*(system.get_kinetic_energy("limb") + system.get_kinetic_energy("mass limb tip")));
        states.e_pot_string.push_back(2.0*(system.get_potential_energy("string") + system.get_potential_energy("mass string tip") + system.get_potential_energy("mass string center")));
        states.e_kin_string.push_back(2.0*(system.get_kinetic_energy("string") + system.get_kinetic_energy("mass string tip") + system.get_kinetic_energy("mass string center")));
        states.e_kin_arrow.push_back(2.0*system.get_kinetic_energy("mass arrow"));

        // Shapes

        states.x_limb.push_back({});
        states.y_limb.push_back({});
        states.x_string.push_back({});
        states.y_string.push_back({});

        for(auto& node: nodes_limb)
        {
            states.x_limb.back().push_back(system.get_u(node.x));
            states.y_limb.back().push_back(system.get_u(node.y));
        }

        for(auto& node: nodes_string)
        {
            states.x_string.back().push_back(system.get_u(node.x));
            states.y_string.back().push_back(system.get_u(node.y));
        }

        // Stresses

        std::vector<double> epsilon(nodes_limb.size());
        std::vector<double> kappa(nodes_limb.size());

        auto elements = system.elements_const().group<BeamElement>("limb");

        for(size_t i = 0; i < nodes_limb.size(); ++i)
        {
            if(i == 0)
            {
                epsilon[i] = elements[i].get_epsilon(0.0);
                kappa[i] = elements[i].get_kappa(0.0);
            }
            else if(i == nodes_limb.size()-1)
            {
                epsilon[i] = elements[i-1].get_epsilon(1.0);
                kappa[i] = elements[i-1].get_kappa(1.0);
            }
            else
            {
                epsilon[i] = 0.5*(elements[i-1].get_epsilon(1.0) + elements[i].get_epsilon(0.0));
                kappa[i] = 0.5*(elements[i-1].get_kappa(1.0) + elements[i].get_kappa(0.0));
            }
        }

        states.sigma_upper.push_back(limb.layers[0].sigma_upper(epsilon, kappa));
        states.sigma_lower.push_back(limb.layers[0].sigma_lower(epsilon, kappa));
    }

private:
    const InputData& input;
    OutputData& output;
    DiscreteLimb limb;

    System system;

    std::vector<Node> nodes_limb;
    std::vector<Node> nodes_string;
    Node node_arrow;
};
