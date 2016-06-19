#pragma once
#include "View.hpp"

struct Node
{
    Dof x;
    Dof y;
    Dof phi;
};

class System
{
private:
    VectorXd u;     // Displacements
    VectorXd v;     // Velocities
    VectorXd a;     // Accelerations
    VectorXd p;     // External forces

    VectorXd uf;    // Displacements of fixed DOFs

public:
    Node create_node(std::array<double, 3> u_node, std::array<DofType, 3> dof_types)
    {
        return Node{create_dof(u_node[0], dof_types[0]),
                    create_dof(u_node[1], dof_types[1]),
                    create_dof(u_node[2], dof_types[2])};
    }

    Dof create_dof(double u_dof, DofType type)
    {
        switch(type)
        {
            case DofType::Active:
            {
                size_t n = u.size() + 1;
                u = (VectorXd(n) << u, u_dof).finished();
                v = (VectorXd(n) << v, 0.0).finished();
                a = (VectorXd(n) << a, 0.0).finished();
                p = (VectorXd(n) << p, 0.0).finished();

                return Dof{type, n-1};
            }
            case DofType::Fixed:
            {
                size_t n = uf.size() + 1;
                uf = (VectorXd(n) << uf, u_dof).finished();

                return Dof{type, n-1};
            }
        }
    }

    size_t dofs() const
    {
        return u.size();
    }

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
};
