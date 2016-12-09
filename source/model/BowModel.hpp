#pragma once
#include "InputData.hpp"
#include "OutputData.hpp"

#include "DiscreteLimb.hpp"
#include "../fem/System.hpp"
#include "../fem/Solver.hpp"
#include "../fem/elements/BeamElement.hpp"
#include "../fem/elements/BarElement.hpp"
#include "../fem/elements/MassElement.hpp"
#include "../numerics/SecantMethod.hpp"
#include "../gui/ProgressDialog.hpp"
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
        node_arrow = nodes_string[0];   //system.create_node({{xc, yc, 0.0}}, {{false, true, false}});

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

        // Store setup results
        output.setup.limb = limb;
        output.setup.string_length = string_length;
    }

    void simulate_statics(TaskState& task)
    {
        BowStates states;

        StaticSolverDC solver(system, nodes_string[0][1], input.operation_draw_length, input.settings_n_draw_steps);
        while(solver.step() && !task.isCanceled())
        {
            get_bow_state(states);
            task.setProgress((nodes_string[0][1].u() - input.operation_brace_height)/
                             (input.operation_draw_length - input.operation_brace_height)*100.0);
        }

        output.statics = std::make_unique<StaticData>(states);
    }

    void simulate_dynamics(TaskState& task)
    {
        // Remove draw force    // Todo: Doesn't really belong in this method
        nodes_string[0][1].p_mut() = 0.0;

        double T = std::numeric_limits<double>::max();
        double alpha = input.settings_time_span_factor;     // Todo: Magic number // Todo: Make this a setting

        DynamicSolver solver(system, input.settings_time_step_factor, input.settings_sampling_time, [&]()
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

            return system.t() >= alpha*T;
        });

        BowStates states;
        while(solver.step() && !task.isCanceled())
        {
            get_bow_state(states);
            task.setProgress(100.0*system.t()/(alpha*T));
        }

        output.dynamics = std::make_unique<DynamicData>(states, *output.statics);
    }

    void get_bow_state(BowStates& states) const
    {
        states.time.push_back(system.t());
        states.draw_force.push_back(2.0*nodes_string[0][1].p());

        states.pos_arrow.push_back(node_arrow[1].u());
        states.vel_arrow.push_back(node_arrow[1].v());
        states.acc_arrow.push_back(node_arrow[1].a());

        states.pos_string.push_back(nodes_string[0][1].u());
        states.vel_string.push_back(nodes_string[0][1].v());
        states.acc_string.push_back(nodes_string[0][1].a());

        states.e_pot_limbs.push_back(2.0*system.get_potential_energy("limb", "mass limb tip"));
        states.e_kin_limbs.push_back(2.0*system.get_kinetic_energy("limb", "mass limb tip"));
        states.e_pot_string.push_back(2.0*system.get_potential_energy("string", "mass string tip", "mass string center"));
        states.e_kin_string.push_back(2.0*system.get_kinetic_energy("string", "mass string tip", "mass string center"));
        states.e_kin_arrow.push_back(2.0*system.get_kinetic_energy("mass arrow"));

        // Shapes

        states.x_limb.push_back({});
        states.y_limb.push_back({});
        states.x_string.push_back({});
        states.y_string.push_back({});

        for(auto& node: nodes_limb)
        {
            states.x_limb.back().push_back(node[0].u());
            states.y_limb.back().push_back(node[1].u());
        }

        for(auto& node: nodes_string)
        {
            states.x_string.back().push_back(node[0].u());
            states.y_string.back().push_back(node[1].u());
        }

        // Stresses

        std::vector<double> epsilon(nodes_limb.size());
        std::vector<double> kappa(nodes_limb.size());

        auto elements = system.element_group<BeamElement>("limb");

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
