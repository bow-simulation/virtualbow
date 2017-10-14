#include "System.hpp"

size_t System::dofs() const
{
    return get_u().size();
}

const VectorXd& System::get_a() const
{
    if(!a_a.is_valid())
    {
        *a_a = get_M().asDiagonal().inverse()*(get_p() - get_q());
        a_a.set_valid();
    }

    return *a_a;
}

const VectorXd& System::get_q() const
{
    if(!q_a.is_valid())
    {
        q_a->conservativeResize(u_a.size());
        q_f->conservativeResize(u_f.size());
        q_a->setZero();
        q_f->setZero();

        for(auto e: elements)
            e->add_internal_forces();

        q_a.set_valid();
    }

    return *q_a;
}

const VectorXd& System::get_M() const
{
    if(!M_a.is_valid())
    {
        M_a->conservativeResize(dofs());
        M_a->setZero();

        for(auto e: elements)
            e->add_masses();

        M_a.set_valid();
    }

    return *M_a;
}

const MatrixXd& System::get_K() const
{
    if(!K_a.is_valid())
    {
        K_a->conservativeResize(dofs(), dofs());
        K_a->setZero();

        for(auto e: elements)
            e->add_tangent_stiffness();

        K_a.set_valid();
    }

    return *K_a;
}

Node System::create_node(std::array<bool, 3> active, std::array<double, 3> u)
{
    return Node({create_dof(active[0], u[0]),
                 create_dof(active[1], u[1]),
                 create_dof(active[2], u[2])});
}

Node System::create_node(const Node& other)
{
    return {create_dof(other.x.active  , get_u(other.x  )),
            create_dof(other.y.active  , get_u(other.y  )),
            create_dof(other.phi.active, get_u(other.phi))};
}

Dof System::create_dof(bool active, double u)
{
    a_a.invalidate();
    q_a.invalidate();
    M_a.invalidate();
    K_a.invalidate();

    if(active)
    {
        size_t n = u_a.size() + 1;
        u_a = (VectorXd(n) << u_a, u).finished();
        v_a = (VectorXd(n) << v_a, 0).finished();
        p_a = (VectorXd(n) << p_a, 0).finished();

        return Dof{active, n-1};
    }
    else
    {
        size_t n = u_f.size() + 1;
        u_f = (VectorXd(n) << u_f, u).finished();

        return Dof{active, n-1};
    }
}
