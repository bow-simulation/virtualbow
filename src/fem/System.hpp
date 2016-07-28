#pragma once
#include "View.hpp"
#include "Node.hpp"
#include "elements/Element.hpp"

#include <Eigen/Eigenvalues>

#include <vector>
#include <iostream>

class System
{
private:
    std::vector<Element*> elements;

    double t;

    VectorXd u;     // Displacements
    VectorXd v;     // Velocities
    VectorXd a;     // Accelerations
    VectorXd p;     // External forces

    VectorXd uf;    // Displacements of fixed DOFs

public:
    System(): t(0.0)
    {

    }

    Node create_node(std::array<double, 3> u_node, std::array<bool, 3> dof_types)
    {
        return Node{create_dof(u_node[0], dof_types[0]),
                    create_dof(u_node[1], dof_types[1]),
                    create_dof(u_node[2], dof_types[2])};
    }

    Dof create_dof(double u_dof, bool type)
    {
        switch(type)
        {
            case true:
            {
                size_t n = u.size() + 1;
                u = (VectorXd(n) << u, u_dof).finished();
                v = (VectorXd(n) << v, 0.0).finished();
                a = (VectorXd(n) << a, 0.0).finished();
                p = (VectorXd(n) << p, 0.0).finished();

                return Dof{type, n-1};
            }
            case false:
            {
                size_t n = uf.size() + 1;
                uf = (VectorXd(n) << uf, u_dof).finished();

                return Dof{type, n-1};
            }
        }
    }

    void add_element(Element& element)
    {
        element.set_state(get_u(), get_v());
        elements.push_back(&element);
    }

    template<class ElementContainer>
    void add_elements(ElementContainer& container)
    {
        for(auto it = container.begin(); it != container.end(); ++it)
        {
            add_element(*it);
        }
    }

    size_t dofs() const
    {
        return u.size();
    }

    double get_time() const
    {
        return t;
    }

    const VectorView<Dof> get_u() const
    {
        return VectorView<Dof>(
        [&](Dof dof)
        {
            if(dof.active)
                return u(dof.index);
            else
                return uf(dof.index);
        },

        nullptr);
    }

    const VectorView<Dof> get_v() const
    {
        return VectorView<Dof>(
        [&](Dof dof)
        {
            if(dof.active)
                return v(dof.index);
            else
                return 0.0;
        },

        nullptr);
    }

    const VectorView<Dof> get_a() const
    {
        return VectorView<Dof>(
        [&](Dof dof)
        {
            if(dof.active)
                return a(dof.index);
            else
                return 0.0;
        },

        nullptr);
    }

    // Todo: Should this be done via a View instead? Problem: Add vs set
    double get_external_force(Dof dof)
    {
        if(dof.active)
            return p(dof.index);
        else
            return 0.0;
    }

    void set_external_force(Dof dof, double val)
    {
        if(dof.active)
            p(dof.index) = val;
    }

    // Euclidean distance between node_a and node_b
    double get_distance(Node node_a, Node node_b)
    {
        double dx = get_u()(node_b.x) - get_u()(node_a.x);
        double dy = get_u()(node_b.y) - get_u()(node_a.y);

        return std::hypot(dx, dy);
    }

    // Angle between the x-axis and the line connecting node_a and node_b
    double get_angle(Node node_a, Node node_b)
    {
        double dx = get_u()(node_b.x) - get_u()(node_a.x);
        double dy = get_u()(node_b.y) - get_u()(node_a.y);

        return std::atan2(dy, dx);
    }

    void solve_statics_lc()
    {
        v.setZero();
        a.setZero();

        unsigned max_iter = 50;     // Todo: Magic number
        double epsilon = 1e-8;      // Todo: Magic number

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
                          const std::function<void()>& callback)
    {
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
                    callback();
                    return iteration;
                }

                ++iteration;
            }

            callback();
            return iteration;
        };

        double init_displacement = u(dof.index);
        double delta_displacement = target_displacement - init_displacement;

        for(unsigned i = 0; i < n_steps; ++i)
        {
            double displacement = init_displacement + delta_displacement*double(i)/double(n_steps - 1);
            solve_equilibrium(displacement);
        }
    }

    void solve_dynamics(double timestep_factor, const std::function<bool()>& callback)
    {
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

public: // Todo: private
    void update_element_states()
    {
        for(auto e: elements)
        {
            e->set_state(get_u(), get_v());
        }
    }

    void get_mass_matrix(VectorXd& M) const
    {
        M.setZero();

        VectorView<Dof> view(
        [&](Dof)
        {
            return 0.0;     // Todo: Unreachable
        },

        [&](Dof dof, double val)
        {
            if(dof.active)
                M(dof.index) += val;
        });

        for(auto e: elements)
        {
            e->get_masses(view);
        }
    }

    void get_internal_forces(VectorXd& q) const
    {
        q.setZero();

        VectorView<Dof> view(
        [&](Dof)
        {
            return 0.0;     // Todo: Unreachable
        },

        [&](Dof dof, double val)
        {
            if(dof.active)
                q(dof.index) += val;
        });

        for(auto e: elements)
        {
            e->get_internal_forces(view);
        }
    }

    void get_tangent_stiffness(MatrixXd& K) const
    {
        K.setZero();

        MatrixView<Dof> view(
        [&](Dof dof_row, Dof dof_col, double val)
        {
            if(dof_row.active && dof_col.active)
                K(dof_row.index, dof_col.index) += val;
        });

        for(auto e: elements)
        {
            e->get_tangent_stiffness(view);
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
};