#include "BowModel.hpp"
#include "model/LimbProperties.hpp"
#include "fem/StaticSolver.hpp"
#include "fem/DynamicSolver.hpp"
#include "fem/elements/BeamElement.hpp"
#include "fem/elements/BarElement.hpp"
#include "fem/elements/MassElement.hpp"
#include "fem/elements/ConstraintElement.hpp"
#include "fem/elements/ContactSurface.hpp"
#include "numerics/RootFinding.hpp"
#include "numerics/Geometry.hpp"

OutputData BowModel::run_static_simulation(const InputData& input, const Callback& callback)
{
    BowModel model(input, callback);

    if(callback(0))
        model.simulate_statics(callback);

    return model.output;
}


OutputData BowModel::run_dynamic_simulation(const InputData& input, const Callback& callback1, const Callback& callback2)
{
    BowModel model(input, callback1);

    if(callback1(0))
        model.simulate_statics(callback1);

    if(callback2(0))
        model.simulate_dynamics(callback2);

    return model.output;
}


BowModel::BowModel(const InputData& input, const Callback& callback)
    : input(input)
{
    init_limb(callback);
    init_string(callback);
    qInfo() << "============== LIMB SETUP SUCCESSFUL ==============";
    init_masses(callback);
}

void BowModel::init_limb(const Callback& callback)
{
    // Calculate discrete limb properties
    output.setup.limb = LimbProperties(input);

    // Create limb nodes
    for(size_t i = 0; i < input.settings_n_elements_limb + 1; ++i)
    {
        bool active = (i != 0);
        Node node = system.create_node({active, active, active}, {output.setup.limb.x[i], output.setup.limb.y[i], output.setup.limb.phi[i]});
        nodes_limb.push_back(node);
    }

    // Create limb elements
    for(size_t i = 0; i < input.settings_n_elements_limb; ++i)
    {
        double rhoA = 0.5*(output.setup.limb.rhoA[i] + output.setup.limb.rhoA[i+1]);
        double L = system.get_distance(nodes_limb[i], nodes_limb[i+1]);

        double Cee = 0.5*(output.setup.limb.Cee[i] + output.setup.limb.Cee[i+1]);
        double Ckk = 0.5*(output.setup.limb.Ckk[i] + output.setup.limb.Ckk[i+1]);
        double Cek = 0.5*(output.setup.limb.Cek[i] + output.setup.limb.Cek[i+1]);

        // Todo: Document this
        double phi = system.get_angle(nodes_limb[i], nodes_limb[i+1]);
        double phi0 = phi - system.get_u(nodes_limb[i].phi);
        double phi1 = phi - system.get_u(nodes_limb[i+1].phi);

        BeamElement element(system, nodes_limb[i], nodes_limb[i+1], rhoA, L);
        element.set_reference_angles(phi0, phi1);
        element.set_stiffness(Cee, Ckk, Cek);
        system.mut_elements().add(element, "limb");
    }
}

void BowModel::init_string(const Callback& callback)
{
    qInfo() << "kmax = " << system.get_K().maxCoeff();
    const double k = system.get_K().maxCoeff();
    const double epsilon = 0.01*output.setup.limb.h[0];    // Transition zone of the contact elements // Magic number

    std::vector<Vector<2>> points;
    points.push_back({0.0, -input.operation_brace_height});
    for(size_t i = 0; i < nodes_limb.size(); ++i)
    {
        points.push_back({
            // Add small initial penetration epsilon to prevent slip-through on the first static solver iteration
            system.get_u(nodes_limb[i].x) + (output.setup.limb.h[i] - epsilon)*sin(system.get_u(nodes_limb[i].phi)),
            system.get_u(nodes_limb[i].y) - (output.setup.limb.h[i] - epsilon)*cos(system.get_u(nodes_limb[i].phi))
        });
    }

    points = constant_orientation_subset(points, true);
    points = equipartition(points, input.settings_n_elements_string + 1);

    // Create string nodes
    for(size_t i = 0; i < points.size(); ++i)
    {
        Node node = system.create_node({i != 0, true, false}, {points[i][0], points[i][1], 0.0});
        nodes_string.push_back(node);
    }

    // Create string elements
    double EA = input.string_n_strands*input.string_strand_stiffness;
    double rhoA = input.string_n_strands*input.string_strand_density;

    for(size_t i = 0; i < input.settings_n_elements_string; ++i)
    {
        BarElement element(system, nodes_string[i], nodes_string[i+1], 0.0, EA, rhoA); // Element lengths are reset later when string length is determined
        system.mut_elements().add(element, "string");
    }

    // Create limb tip constraint and string to limb contact surface
    //double k = EA/output.setup.string_length;    // Stiffness estimate based on string stiffness

    //double k = output.setup.limb.Cee[0]/(output.setup.limb.s[1] - output.setup.limb.s[0]);    // Stiffness estimate based on limb data
    system.mut_elements().add(ConstraintElement(system, nodes_limb.back(), nodes_string.back(), k), "constraint");
    system.mut_elements().add(ContactSurface(system, nodes_limb, {nodes_string.begin(), nodes_string.end()-1},    // Todo: Unnecessary copy
                                             output.setup.limb.h, {k, epsilon}), "contact");

    // Function that sets the sting element length and returns the
    // resulting difference between actual and desired brace height
    /*
    StaticSolverLC solver(system);
    auto try_element_length = [&](double l)
    {
        for(auto& element: system.mut_elements().group<BeamElement>("string"))
            element.set_length(l);

        qInfo() << "l = " << l;

        solver.solve();
        return system.get_u(nodes_string[0].y) + input.operation_brace_height;    // Todo: Use dimensionless measure
    };
    */

    // Takes a string element length, iterates to equilibrium with the constraint of the brace height
    // and returns the angle of the string center
    system.set_p(nodes_string[0].y, 1.0);    // Will be scaled by the static algorithm
    StaticSolverDC solver(system, nodes_string[0].y);   // Todo: Reuse solver across function calls?
    StaticSolverDC::Info info;
    auto try_element_length = [&](double l)
    {
        for(auto& element: system.mut_elements().group<BarElement>("string"))
            element.set_length(l);

        info = solver.solve(-input.operation_brace_height);
        return system.get_angle(nodes_string[0], nodes_string[1]);
    };


    // Find a element length at which the brace height difference is zero
    // Todo: Perhaps limit the step size of the root finding algorithm to increase robustness.
    double l = (points[1] - points[0]).norm();
    if(!try_element_length(l) > 0)
        throw std::runtime_error("Invalid input: Brace height is too low");

    double dl = 1e-3*l;        // Initial step length, later adjusted by the algorithm    // Magic number
    double dl_min = 1e-5*l;   // Minimum step length, abort if smaller                   // Magic number
    unsigned iterations = 5;    // Desired number of iterations for the static solver      // Magic number
    while(callback(0))
    {
        // Try length = l - dl
        double alpha = try_element_length(l - dl);

        if(info.outcome == StaticSolverDC::Info::Success)
        {
            // Success: Apply step.
            l -= dl;

            // If sign change of alpha: Almost done, do the rest by root finding.
            if(alpha <= 0.0)
            {
                l = bisect<true>(try_element_length, l, l+dl, 1e-5, 1e-10, 20);    // Magic numbers
                break;
            }

            // Adjust step length
            dl *= double(iterations)/info.iterations;
        }
        else
        {
            // Reduce step length by generic factor
            dl *= 0.5;
        }

        if(dl < dl_min)
            throw std::runtime_error("Bracing failed: Step size too small");
    }

    // Assign setup data
    output.setup.string_length = 2.0*l*input.settings_n_elements_string;    // *2 because of symmetry
}

void BowModel::init_masses(const Callback& callback)
{
    node_arrow = nodes_string[0];
    MassElement mass_limb_tip(system, nodes_limb.back(), input.mass_limb_tip);
    MassElement mass_string_tip(system, nodes_string.back(), input.mass_string_tip);
    MassElement mass_string_center(system, nodes_string.front(), 0.5*input.mass_string_center);   // 0.5 because of symmetry
    MassElement mass_arrow(system, node_arrow, 0.5*input.operation_mass_arrow);                   // 0.5 because of symmetry

    system.mut_elements().add(mass_limb_tip, "limb tip");
    system.mut_elements().add(mass_string_tip, "string tip");
    system.mut_elements().add(mass_string_center, "string center");
    system.mut_elements().add(mass_arrow, "arrow");
}

void BowModel::simulate_statics(const Callback& callback)
{
    system.set_p(nodes_string[0].y, 1.0);    // Will be scaled by the static algorithm
    StaticSolverDC solver(system, nodes_string[0].y);
    for(unsigned i = 0; i < input.settings_n_draw_steps; ++i)
    {
        double eta = double(i)/(input.settings_n_draw_steps - 1);
        double draw_length = (1.0 - eta)*input.operation_brace_height + eta*input.operation_draw_length;

        solver.solve(-draw_length);
        add_state(output.statics.states);

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

// Todo: Calculate timestep only once, use for both parts of the dynamic simulation

void BowModel::simulate_dynamics(const Callback& callback)
{
    // Set draw force to zero
    system.set_p(nodes_string[0].y, 0.0);

    double dt = DynamicSolver::estimate_timestep(system, input.settings_time_step_factor);
    double T = std::numeric_limits<double>::max();
    double alpha = input.settings_time_span_factor;

    DynamicSolver solver1(system, dt, input.settings_sampling_rate, [&] {
        double ut = system.get_u(node_arrow.y);
        if(ut < input.operation_brace_height)
        {
            double u0 = -input.operation_draw_length;
            double u1 = -input.operation_brace_height;

            if(ut != u0)
                T = system.get_t()*std::acos(u1/u0)/std::acos(ut/u0);
        }

        return system.get_a(node_arrow.y) <= 0;
    });

    auto run_solver = [&](DynamicSolver& solver) {
        while(solver.step())
        {
            add_state(output.dynamics.states);
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

    DynamicSolver solver2(system, dt, input.settings_sampling_rate, [&] {
        return system.get_t() >= alpha*T;
    });

    run_solver(solver2);

    // Calculate scalar values

    output.dynamics.final_arrow_velocity = output.dynamics.states.vel_arrow.back();
    output.dynamics.final_arrow_energy = output.dynamics.states.e_kin_arrow.back();
    output.dynamics.efficiency = output.dynamics.final_arrow_energy/output.statics.drawing_work;
}

void BowModel::add_state(BowStates& states) const
{
    states.time.push_back(system.get_t());
    states.draw_force.push_back(-2.0*system.get_p(nodes_string[0].y));    // *2 because of symmetry
    states.draw_length.push_back(-system.get_u(nodes_string[0].y));

    states.pos_arrow.push_back(system.get_u(node_arrow.y));
    states.vel_arrow.push_back(system.get_v(node_arrow.y));
    states.acc_arrow.push_back(system.get_a(node_arrow.y));

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


    // Limb and string coordinates
    states.x_limb.push_back(VectorXd(nodes_limb.size()));
    states.y_limb.push_back(VectorXd(nodes_limb.size()));
    states.phi_limb.push_back(VectorXd(nodes_limb.size()));

    for(size_t i = 0; i < nodes_limb.size(); ++i)
    {
        states.x_limb.back()[i] = system.get_u(nodes_limb[i].x);
        states.y_limb.back()[i] = system.get_u(nodes_limb[i].y);
        states.phi_limb.back()[i] = system.get_u(nodes_limb[i].phi);
    }

    states.x_string.push_back(VectorXd(nodes_string.size()));
    states.y_string.push_back(VectorXd(nodes_string.size()));

    for(size_t i = 0; i < nodes_string.size(); ++i)
    {
        states.x_string.back()[i] = system.get_u(nodes_string[i].x);
        states.y_string.back()[i] = system.get_u(nodes_string[i].y);
    }

    // Stresses

    VectorXd epsilon(nodes_limb.size());
    VectorXd kappa(nodes_limb.size());

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

    states.sigma_back.push_back(output.setup.limb.layers[0].sigma_back(epsilon, kappa));
    states.sigma_belly.push_back(output.setup.limb.layers[0].sigma_belly(epsilon, kappa));
}
