#pragma once
#include "View.hpp"
#include "Node.hpp"
#include "elements/Element.hpp"
#include "ElementContainer.hpp"
#include <Eigen/Eigenvalues>
#include <vector>

#include <iostream>

class System
{
private:
    ElementContainer m_elements;

    double t;       // Time
    VectorXd u;     // Displacements
    VectorXd v;     // Velocities
    VectorXd a;     // Accelerations
    VectorXd p;     // External forces

    VectorXd uf;    // Displacements of fixed DOFs

public:
    VectorView<Dof> get_u;    // Todo: Rename to view_u, view_v, view_a or something better?
    VectorView<Dof> get_v;
    VectorView<Dof> get_a;
    VectorView<Dof> get_p;

    System()
        : t(0.0),
          get_u([&](Dof dof){ return dof.active ? u(dof.index) : uf(dof.index); }, nullptr, nullptr),
          get_v([&](Dof dof){ return dof.active ? v(dof.index) : 0.0;           }, nullptr, nullptr),
          get_a([&](Dof dof){ return dof.active ? a(dof.index) : 0.0;           }, nullptr, nullptr),
          get_p([&](Dof dof){ return dof.active ? p(dof.index) : 0.0;           }, [&](Dof dof, double val){ if(dof.active) p(dof.index) = val; }, nullptr)
    {

    }

    Node create_node(std::array<bool, 3> active, std::array<double, 3> u_node, std::array<double, 3> v_node = {0.0, 0.0, 0.0})
    {
        return Node{create_dof(u_node[0], v_node[0], active[0]),
                    create_dof(u_node[1], v_node[1], active[1]),
                    create_dof(u_node[2], v_node[2], active[2])};
    }

    // Create node with the same displacement and velocity as 'other'
    Node create_node(const Node& other, std::array<bool, 3> active)
    {
        return Node{create_dof(get_u(other.x),   get_v(other.x),   active[0]),
                    create_dof(get_u(other.y),   get_v(other.y),   active[1]),
                    create_dof(get_u(other.phi), get_v(other.phi), active[2])};
    }

    Dof create_dof(double u_dof, double v_dof, bool active)
    {
        if(active)
        {
            size_t n = u.size() + 1;
            u = (VectorXd(n) << u, u_dof).finished();
            v = (VectorXd(n) << v, v_dof).finished();
            a = (VectorXd(n) << a, 0.0).finished();
            p = (VectorXd(n) << p, 0.0).finished();

            return Dof{active, n-1};
        }
        else
        {
            size_t n = uf.size() + 1;
            uf = (VectorXd(n) << uf, u_dof).finished();

            return Dof{active, n-1};
        }
    }

    ElementContainer& elements()
    {
        return m_elements;
    }

    const ElementContainer& elements_const() const
    {
        return m_elements;
    }

    size_t dofs() const
    {
        return u.size();
    }

    double get_time() const
    {
        return t;
    }

    // Euclidean distance between node_a and node_b
    double get_distance(Node node_a, Node node_b)
    {
        double dx = get_u(node_b.x) - get_u(node_a.x);
        double dy = get_u(node_b.y) - get_u(node_a.y);

        return std::hypot(dx, dy);
    }

    // Angle between the x-axis and the line connecting node_a to node_b
    double get_angle(Node node_a, Node node_b)
    {
        double dx = get_u(node_b.x) - get_u(node_a.x);
        double dy = get_u(node_b.y) - get_u(node_a.y);

        return std::atan2(dy, dx);
    }

    /*
    template<typename... Args>
    double get_kinetic_energy(const std::string& key, Args... args) const
    {
        double e_kin = 0.0;
        for(auto& e: m_elements.group(key))
        {
            e_kin += e.get_kinetic_energy(get_v);
        }

        return e_kin + get_kinetic_energy(args...);
    }
    */

    template<typename... Args>
    double get_kinetic_energy(const std::string& key, Args... args) const
    {
        return get_kinetic_energy(key) + get_kinetic_energy(args...);
    }

    double get_kinetic_energy(const std::string& key) const
    {
        double e_kin = 0.0;
        for(auto& e: m_elements.group(key))
        {
            e_kin += e.get_kinetic_energy(get_v);
        }

        return e_kin;
    }

    template<typename... Args>
    double get_potential_energy(const std::string& key, Args... args) const
    {
        return get_potential_energy(key) + get_potential_energy(args...);
    }

    double get_potential_energy(const std::string& key) const
    {
        double e_pot = 0.0;
        for(auto& e: m_elements.group(key))
        {
            e_pot += e.get_potential_energy();
        }

        return e_pot;
    }

    // Todo: Rename statics methods to 'solve_equilibrium' and 'solve_equilibrium_path'?
    void solve_statics_lc()
    {
        // Todo: Remove and replace with comprehensive approach to changing data and recalculation
        update_element_states();

        v.setZero();
        a.setZero();

        unsigned max_iter = 50;     // Todo: Magic number
        double epsilon = 1e-6;      // Todo: Magic number

        Eigen::LDLT<Eigen::MatrixXd> stiffness_dec;
        Eigen::MatrixXd K(dofs(), dofs());
        Eigen::VectorXd q(dofs());

        get_internal_forces(q);
        get_tangent_stiffness(K);

        double norm_0 = (p - q).norm();

        if(norm_0 == 0)     // Todo: Introduce absolute tolerance for this
        {
            return;   // System already in equilibrium
        }

        for(unsigned i = 0; i < max_iter; ++i)
        {
            stiffness_dec.compute(K);
            if(stiffness_dec.info() != Eigen::Success)
            {
                throw std::runtime_error("Stiffness matrix decomposition failed");
            }

            u += stiffness_dec.solve(p - q);
            update_element_states();

            double norm_i = (p - q).norm();
            if(norm_i/norm_0 < epsilon)
            {
                return;
            }

            get_internal_forces(q);
            get_tangent_stiffness(K);
        }

        throw std::runtime_error("Maximum number of iterations exceeded");
    }

    void solve_statics_dc(Dof dof,
                          double target_displacement,
                          unsigned n_steps,
                          const std::function<bool()>& callback = [](){ return true; })
    {
        // Todo: Remove and replace with comprehensive approach to changing data and recalculation
        update_element_states();

        v.setZero();
        a.setZero();

        unsigned max_iter = 50;     // Todo: Magic number
        double epsilon = 1e-8;      // Todo: Magic number

        if(!dof.active)
        {
            throw std::runtime_error("Displacement control not possible for a fixed DOF");
        }

        Eigen::VectorXd q(dofs());
        Eigen::MatrixXd K(dofs(), dofs());
        Eigen::VectorXd delta_q(dofs());

        Eigen::VectorXd alpha(dofs());
        Eigen::VectorXd beta(dofs());
        Eigen::VectorXd e = Eigen::VectorXd::Zero(dofs());
        e(dof.index) = 1.0;

        Eigen::LDLT<Eigen::MatrixXd> stiffness_dec;

        // Todo: Think of a better control flow
        auto solve_equilibrium = [&](double displacement)
        {
            unsigned iteration = 0;
            while(iteration < max_iter)
            {
                if(iteration == max_iter - 1)
                {
                    throw std::runtime_error("Maximum number of iterations exceeded");
                }

                // Calculate stiffness matrix and out of balance loads
                get_internal_forces(q);
                get_tangent_stiffness(K);
                delta_q = p - q;

                stiffness_dec.compute(K);
                if(stiffness_dec.info() != Eigen::Success)
                {
                    throw std::runtime_error("Decomposition of the stiffness matrix failed");
                }

                alpha = stiffness_dec.solve(delta_q);
                beta = stiffness_dec.solve(e);
                double delta_f = (displacement - u(dof.index) - alpha(dof.index))/beta(dof.index);

                u += alpha + beta*delta_f;
                p(dof.index) += delta_f;
                update_element_states();

                if((alpha + delta_f*beta).norm()/double(dofs()) < epsilon)    // Todo: Better convergence criterion
                {
                    return iteration;
                }

                ++iteration;
            }

            return iteration;
        };

        double init_displacement = u(dof.index);

        // Todo: Separate into different methods
        // Todo: Think about meaning of 'n_steps'. Number of times the newton method is employed? Or number of intervals in between?
        double delta_displacement = target_displacement - init_displacement;
        for(unsigned i = 0; i <= n_steps; ++i)
        {
            double displacement = init_displacement + delta_displacement*double(i)/double(n_steps);
            solve_equilibrium(displacement);

            if(!callback())
                return;
        }
    }

    void solve_dynamics(double timestep_factor, const std::function<bool()>& callback)
    {
        // Todo: Remove and replace with comprehensive approach to changing data and recalculation
        update_element_states();

        VectorXd M(dofs());
        VectorXd q(dofs());
        MatrixXd K(dofs(), dofs());

        get_mass_matrix(M);
        get_internal_forces(q);
        get_tangent_stiffness(K);

        // Timestep estimation
        Eigen::GeneralizedSelfAdjointEigenSolver<Eigen::MatrixXd> eigen_solver(K, M.asDiagonal(), Eigen::DecompositionOptions::EigenvaluesOnly);
        if(eigen_solver.info() != Eigen::Success)
        {
            throw std::runtime_error("Failed to compute eigenvalues of the system");
        }

        double omega_max = std::sqrt(eigen_solver.eigenvalues().maxCoeff());
        double dt = timestep_factor*2.0/omega_max;

        // Version with a more accurate velocity based on central differences
        a = M.asDiagonal().inverse()*(p - q);

        // Previous displacements
        Eigen::VectorXd u_p2(dofs());
        Eigen::VectorXd u_p1 = u - dt*v + dt*dt/2.0*a;

        while(callback())
        {
            u_p2 = u_p1;
            u_p1 = u;

            u = 2*u - u_p2 + dt*dt*a;
            t += dt;

            update_element_states();
            get_internal_forces(q);
            v = (u - u_p2)/(2.0*dt) + dt*a;
            a = M.asDiagonal().inverse()*(p - q);
        }
    }

    // Todo: tangent stiffness methods only need to be public for tests. Wat do?
    void get_tangent_stiffness(MatrixXd& K) const
    {
        K.setZero();

        MatrixView<Dof> view(
        [&](Dof dof_row, Dof dof_col, double val)
        {
            if(dof_row.active && dof_col.active)
                K(dof_row.index, dof_col.index) += val;
        });

        for(auto& e: m_elements)
        {
            e.get_tangent_stiffness(view);
        }
    }

    // Numeric stiffness matrix via central difference quotient. Only for testing purposes.
    void get_numeric_tangent_stiffness(MatrixXd& K, double h)
    {
        K.setZero();

        Eigen::VectorXd q_fwd = Eigen::VectorXd::Zero(dofs());
        Eigen::VectorXd q_bwd = Eigen::VectorXd::Zero(dofs());

        for(std::size_t i = 0; i < dofs(); ++i)
        {
            double ui = u(i);

            u(i) = ui + h;
            update_element_states();
            get_internal_forces(q_fwd);

            u(i) = ui - h;
            update_element_states();
            get_internal_forces(q_bwd);

            u(i) = ui;

            K.col(i) = (q_fwd - q_bwd)/(2.0*h);
        }
    }

private:
    void update_element_states()
    {
        for(auto& e: m_elements)
        {
            e.set_state(get_u, get_v);
        }
    }

    void get_mass_matrix(VectorXd& M) const
    {
        M.setZero();

        VectorView<Dof> view(nullptr, nullptr,
        [&](Dof dof, double val)
        {
            if(dof.active)
                M(dof.index) += val;
        });

        for(auto& e: m_elements)
        {
            e.get_masses(view);
        }
    }

    void get_internal_forces(VectorXd& q) const
    {
        q.setZero();

        VectorView<Dof> view(nullptr, nullptr,
        [&](Dof dof, double val)
        {
            if(dof.active)
                q(dof.index) += val;
        });

        for(auto& e: m_elements)
        {
            e.get_internal_forces(view);
        }
    }
};
