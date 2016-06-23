#pragma once
#include "View.hpp"
#include "Node.hpp"
#include "elements/Element.hpp"

#include <vector>
#include <iostream>

class System
{
private:
    std::vector<const Element*> elements;

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

    void add_element(const Element& element)
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

        [&](Dof, double)
        {
            // Todo: Unreachable
        });
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

        [&](Dof, double)
        {
            // Todo: Unreachable
        });
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

        [&](Dof, double)
        {
            // Todo: Unreachable
        });
    }

    /*
    VectorView<Dof> get_u()
    {
        return VectorView<Dof>(
        [&](Dof dof)
        {
            if(dof.type == Dof::Type::Active)
                return u(dof.index);
            else
                return uf(dof.index);
        },

        [&](Dof dof, double val)
        {
            if(dof.type == Dof::Type::Active)
                u(dof.index) = val;
            else
                uf(dof.index) = val;
        });
    }
    */

    /*
    const VectorView<const VectorXd> get_u() const
    {
        VectorAccess<const VectorXd> access(&u, &uf);
        return VectorView<const VectorXd>(access);
    }

    const VectorView<const VectorXd> get_v() const
    {
        VectorAccess<const VectorXd> access(&v, nullptr);
        return VectorView<const VectorXd>(access);
    }

    const VectorView<const VectorXd> get_a() const
    {
        VectorAccess<const VectorXd> access(&a, nullptr);
        return VectorView<const VectorXd>(access);
    }

    VectorView<VectorXd> get_p()
    {
        VectorAccess<VectorXd> access(&p, nullptr);
        return VectorView<VectorXd>(access);
    }

    const VectorView<const VectorXd> get_p() const
    {
        VectorAccess<const VectorXd> access(&p, nullptr);
        return VectorView<const VectorXd>(access);
    }
    */

    void solve_dynamics(double dt, const std::function<bool()>& callback)
    {
        VectorXd M(dofs());
        VectorXd q(dofs());
        MatrixXd K(dofs(), dofs());

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

            get_internal_forces(q);
            v = (u - u_p2)/(2.0*dt) + dt*a;
            a = M.asDiagonal().inverse()*(p - q);
        }
    }

private:
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
                M(dof.index) = val;
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
                q(dof.index) = val;
        });

        for(auto e: elements)
        {
            e->get_internal_forces(get_u(), get_v(), view);
        }
    }
};
