#pragma once
#include "InputData2.hpp"
#include "OutputData.hpp"
#include "DiscreteLimb.hpp"

#include "../fem/System.hpp"
#include "../fem/elements/BeamElement.hpp"
#include "../fem/elements/BarElement.hpp"
#include "../fem/elements/MassElement.hpp"
#include "../fem/elements/ContactElement1D.hpp"
#include "../numerics/SecantMethod.hpp"

#include <QtCore>

class BowModel
{
public:
    static OutputData simulate(const InputData2& input, bool statics, bool dynamics)
    {
        BowModel model(input);

        // Todo: Omit detailed static simulation if statics == false
        OutputData output;
        output.setup = model.simulate_setup();
        model.simulate_statics();

        if(dynamics)
            model.simulate_dynamics();

        return output;
    }

private:
    const InputData2& input;
    DiscreteLimb limb;

    System system;

    std::vector<BeamElement> elements_limb;
    std::vector<BarElement> elements_string;
    std::vector<Node> nodes_limb;
    std::vector<Node> nodes_string;
    Node node_arrow;

    MassElement mass_limb_tip;
    MassElement mass_string_tip;
    MassElement mass_string_center;
    MassElement mass_arrow;

    ContactElement1D contact_arrow;

    BowModel(const InputData2& input)
        : input(input),
          limb(input)
    {

    }

    BowSetup simulate_setup()
    {
        size_t n = input.settings.n_elements_limb;
        size_t k = input.settings.n_elements_string;

        // Create limb nodes
        for(size_t i = 0; i < n+1; ++i)
        {
            bool active = (i != 0);
            Node node = system.create_node({{limb.x[i], limb.y[i], limb.phi[i]}}, {{active, active, active}});
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

            // Todo: Document2 this
            double phi = system.get_angle(nodes_limb[i], nodes_limb[i+1]);
            double phi0 = phi - system.get_u(nodes_limb[i].phi);
            double phi1 = phi - system.get_u(nodes_limb[i+1].phi);

            BeamElement element(nodes_limb[i], nodes_limb[i+1], rhoA, L);
            element.set_reference_angles(phi0, phi1);
            element.set_stiffness(Cee, Ckk, Cek);
            elements_limb.push_back(element);
        }

        // Limb tip
        double xt = system.get_u(nodes_limb.back().x);
        double yt = system.get_u(nodes_limb.back().y);

        // String center at brace height
        double xc = input.operation.brace_height;
        double yc = 0.0;

        // Create string nodes
        for(size_t i = 0; i < k; ++i)
        {
            double p = double(i)/double(k);
            double x = xc*(1.0 - p) + xt*p;
            double y = yc*(1.0 - p) + yt*p;

            Node node = system.create_node({{x, y, 0.0}}, {{true, (i != 0), false}});
            nodes_string.push_back(node);
        }
        nodes_string.push_back(nodes_limb.back());

        // Create arrow node
        node_arrow = system.create_node({{xc, yc, 0.0}}, {{true, false, false}});

        // Create string elements
        double rhoA = double(input.string.n_strands)*input.string.strand_density;
        double EA = double(input.string.n_strands)*input.string.strand_stiffness;

        for(size_t i = 0; i < k; ++i)
        {
            BarElement element(nodes_string[i], nodes_string[i+1], 0.0, EA, 0.0, rhoA); // Element lengths are reset later when string length is determined
            elements_string.push_back(element);
        }

        // Todo: Add elements inside loops, prevent iterator invalidation by using unique_ptr or shared_ptr
        // or maybe even let system own elements. Look at boost pointer container library.
        system.add_elements(elements_limb);
        system.add_elements(elements_string);

        // Create mass elements
        // MassElement(Node nd, double m, double I)
        mass_limb_tip = MassElement(nodes_limb.back(), input.limb.tip_mass);
        mass_string_tip = MassElement(nodes_string.back(), input.string.end_mass);
        mass_string_center = MassElement(nodes_string.front(), 0.5*input.string.center_mass);   // 0.5 because of symmetric model
        mass_arrow = MassElement(node_arrow, 0.5*input.operation.arrow_mass);         // 0.5 because of symmetric model

        // Arrow contact with default values for static analysis
        contact_arrow = ContactElement1D(node_arrow, nodes_string[0], 1e5, 0.0); // Todo: Magic numbers

        system.add_element(mass_limb_tip);
        system.add_element(mass_string_tip);
        system.add_element(mass_string_center);
        system.add_element(mass_arrow);
        system.add_element(contact_arrow);

        // Takes a string length, iterates to equilibrium with the constraint of the brace height
        // and returns the angle of the string center
        auto try_string_length = [&](double string_length)
        {
            double L = 0.5*string_length/double(k);
            for(auto& element: elements_string)
            {
                element.set_length(L);
            }

            Dof dof = nodes_string[0].x;
            system.solve_statics_dc(dof, xc, 1, [](){});

            return system.get_angle(nodes_string[0], nodes_string[1]) - M_PI/2;
        };

        // Todo: Perhaps limit the step size of the root finding algorithm ti increase robustness.
        double string_length = 2.0*std::hypot(xc - xt, yc - yt);
        string_length = secant_method(try_string_length, 0.95*string_length, 0.9*string_length, 1e-8, 50);   // Todo: Magic numbers

        // Store setup results
        BowSetup setup;
        setup.limb = limb;
        setup.string_length = string_length;

        return setup;
    }

    BowStates simulate_statics()
    {
        BowStates states;
        system.solve_statics_dc(nodes_string[0].x, input.operation.draw_length, 50, [&]()   // Todo: Magic number
        {
            get_bow_state(states);
        });

        qInfo() << "===============";

        return states;
    }

    BowStates simulate_dynamics()
    {
        // Adjust arrow contact based on static results
        double max_penetration = 1e-4*(input.operation.draw_length - input.operation.brace_height);  // Todo: Magic number
        double max_force = system.get_p(nodes_string[0].x);    // Todo: Better way to get static results, maybe pass reference.    // Todo: Assumes that max draw force is reached at end of draw
        double kc = max_force/max_penetration;

        double ml = input.operation.arrow_mass;
        double mr = input.string.center_mass + elements_string[0].get_node_mass();  // Todo: Find better (less fragile) way to get this mass
        double dc = 2.0*std::sqrt(kc*ml*mr/(ml + mr));

        contact_arrow.set_stiffness(kc);
        contact_arrow.set_damping(dc);
        contact_arrow.set_one_sided(true);

        // Remove draw force    // Todo: Doesn't really belong in this method
        system.get_p(nodes_string[0].x) = 0.0;

        double T = 0.05;
        double t = 0.0;
        double dt = 1e-4;

        BowStates states;
        system.solve_dynamics(input.settings.step_factor, [&]()
        {
            if(system.get_time() > t + dt)
            {
                get_bow_state(states);
                t = system.get_time();
            }

            return t < T;
        });

        return states;
    }

    void get_bow_state(BowStates& states) const
    {
        states.time.push_back(system.get_time());
        states.draw_force.push_back(system.get_p(nodes_string[0].x));
        states.pos_string.push_back(system.get_u(nodes_string[0].x));
        states.pos_arrow.push_back(system.get_u(node_arrow.x));

        //qInfo() << states.pos_string.back() << ", " << states.draw_force.back();
        qInfo() << states.time.back() << ", " << states.pos_arrow.back() - states.pos_string.back();
    }

};

/*
for(auto& node: nodes_limb)
{
    qInfo() << system.get_u()(node.x) << ", " << system.get_u()(node.y);
}

qInfo() << "=============";

for(auto& node: nodes_string)
{
    qInfo() << system.get_u()(node.x) << ", " << system.get_u()(node.y);
}
*/
