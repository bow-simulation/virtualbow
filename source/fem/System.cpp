#include "System.hpp"

size_t System::dofs() const
{
    return get_u().size();
}

const VectorXd& System::get_a() const
{
    if(!m_a.is_valid())
    {
        *m_a = get_M().asDiagonal().inverse()*(get_p() - get_q());
        m_a.set_valid();
    }

    return *m_a;
}

const VectorXd& System::get_q() const
{
    if(!m_q.is_valid())
    {
        m_q->conservativeResize(dofs());
        m_q->setZero();

        for(auto e: elements)
            e->add_internal_forces();

        m_q.set_valid();
    }

    return *m_q;
}

const VectorXd& System::get_M() const
{
    if(!m_M.is_valid())
    {
        m_M->conservativeResize(dofs());
        m_M->setZero();

        for(auto e: elements)
            e->add_masses();

        m_M.set_valid();
    }

    return *m_M;
}

const MatrixXd& System::get_K() const
{
    if(!m_K.is_valid())
    {
        m_K->conservativeResize(dofs(), dofs());
        m_K->setZero();

        for(auto e: elements)
            e->add_tangent_stiffness();

        m_K.set_valid();
    }

    return *m_K;
}

Node System::create_node(std::array<bool, 3> active, std::array<double, 3> u_node, std::array<double, 3> v_node)
{
    return Node({create_dof(active[0], u_node[0], v_node[0]),
                 create_dof(active[1], u_node[1], v_node[1]),
                 create_dof(active[2], u_node[2], v_node[2])});
}

Node System::create_node(const Node& other)
{
    return {create_dof(other.x.m_active  , get_u(other.x  ), get_v(other.x  )),
            create_dof(other.y.m_active  , get_u(other.y  ), get_v(other.y  )),
            create_dof(other.phi.m_active, get_u(other.phi), get_v(other.phi))};
}

Dof System::create_dof(bool active, double u_dof, double v_dof)
{
    if(active)
    {
        size_t n = dofs() + 1;
        set_u((VectorXd(n) << get_u(), u_dof).finished());
        set_v((VectorXd(n) << get_v(), v_dof).finished());
        set_p((VectorXd(n) << get_p(), 0.0).finished());

        return Dof(dofs()-1);
    }
    else
    {
        return Dof(u_dof);
    }
}
