#pragma once
#include "View.hpp"
#include "Node.hpp"
#include "elements/Element.hpp"

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

    Node create_node(std::array<double, 3> u_node, std::array<Dof::Type, 3> dof_types)
    {
        return Node{create_dof(u_node[0], dof_types[0]),
                    create_dof(u_node[1], dof_types[1]),
                    create_dof(u_node[2], dof_types[2])};
    }

    Dof create_dof(double u_dof, Dof::Type type)
    {
        switch(type)
        {
            case Dof::Type::Active:
            {
                size_t n = u.size() + 1;
                u = (VectorXd(n) << u, u_dof).finished();
                v = (VectorXd(n) << v, 0.0).finished();
                a = (VectorXd(n) << a, 0.0).finished();
                p = (VectorXd(n) << p, 0.0).finished();

                return Dof{type, n-1};
            }
            case Dof::Type::Fixed:
            {
                size_t n = uf.size() + 1;
                uf = (VectorXd(n) << uf, u_dof).finished();

                return Dof{type, n-1};
            }
        }
    }

    void add_element(Element& element)
    {
        elements.push_back(&element);
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
            if(dof.type == Dof::Type::Active)
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
            if(dof.type == Dof::Type::Active)
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
            if(dof.type == Dof::Type::Active)
                return a(dof.index);
            else
                return 0.0;
        },

        nullptr);
    }

    VectorView<Dof> get_p()
    {
        return VectorView<Dof>(
        [&](Dof dof)
        {
            if(dof.type == Dof::Type::Active)
                return p(dof.index);
            else
                return 0.0;
        },

        [&](Dof dof, double val)
        {
            if(dof.type == Dof::Type::Active)
                p(dof.index) += val;
        });
    }

    void solve_dynamics(double dt, const std::function<bool()>& callback)
    {
        VectorXd M(dofs());
        VectorXd q(dofs());
        MatrixXd K(dofs(), dofs());

        update_element_states();
        get_mass_matrix(M);
        get_internal_forces(q);

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

    void get_mass_matrix(VectorXd& M)
    {
        M.setZero();

        VectorView<Dof> view(
        [&](Dof)
        {
            return 0.0;     // Todo: Unreachable
        },

        [&](Dof dof, double val)
        {
            if(dof.type == Dof::Type::Active)
                M(dof.index) += val;
        });

        for(auto e: elements)
        {
            e->get_masses(view);
        }
    }

    void get_internal_forces(VectorXd& q)
    {
        q.setZero();

        VectorView<Dof> view(
        [&](Dof)
        {
            return 0.0;     // Todo: Unreachable
        },

        [&](Dof dof, double val)
        {
            if(dof.type == Dof::Type::Active)
                q(dof.index) += val;
        });

        for(auto e: elements)
        {
            e->get_internal_forces(view);
        }
    }

    void get_tangent_stiffness(MatrixXd& K)
    {
        K.setZero();

        MatrixView<Dof> view(
        [&](Dof dof_row, Dof dof_col, double val)
        {
            if(dof_row.type == Dof::Type::Active && dof_col.type == Dof::Type::Active)
                K(dof_row.index, dof_col.index) += val;
        });

        for(auto e: elements)
        {
            e->get_tangent_stiffness(view);
        }
    }
};
