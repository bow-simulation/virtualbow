#include "System.hpp"

size_t System::dofs() const
{
    return u().size();
}

double& System::t_mut()
{
    return m_t;
}

VectorXd& System::u_mut()
{
    m_a.set_valid(false);
    m_q.set_valid(false);
    m_K.set_valid(false);

    return m_u;
}

VectorXd& System::v_mut()
{
    m_a.set_valid(false);
    m_q.set_valid(false);

    return m_v;
}

VectorXd& System::p_mut()
{
    m_a.set_valid(false);

    return m_p;
}

const double& System::t() const
{
    return m_t;
}

const VectorXd& System::u() const
{
    return m_u;
}


const VectorXd& System::v() const
{
    return m_v;
}

const VectorXd& System::p() const
{
    return m_p;
}

const VectorXd& System::a() const
{
    if(!m_a.is_valid())
    {
        m_a.set_valid(true);
        *m_a = M().asDiagonal().inverse()*(p() - q());
    }

    return *m_a;
}

const VectorXd& System::q() const
{
    if(!m_q.is_valid())
    {
        m_q.set_valid(true);
        m_q->conservativeResize(dofs());
        m_q->setZero();

        VectorView view(nullptr, nullptr,
        [&](Dof dof, double val)
        {
            if(dof.type() == DofType::Active)
                (*m_q)(dof.index()) += val;
        });

        for(auto e: elements)
        {
            e->update_state();
            e->get_internal_forces(view);
        }
    }

    return *m_q;
}

const VectorXd& System::M() const
{
    if(!m_M.is_valid())
    {
        m_M.set_valid(true);
        m_M->conservativeResize(dofs());
        m_M->setZero();

        VectorView view(nullptr, nullptr,
        [&](Dof dof, double val)
        {
            if(dof.type() == DofType::Active)
                (*m_M)(dof.index()) += val;
        });

        for(auto e: elements)
        {
            e->get_masses(view);
        }
    }

    return *m_M;
}

const MatrixXd& System::K() const
{
    if(!m_K.is_valid())
    {
        m_K.set_valid(true);
        m_K->conservativeResize(dofs(), dofs());
        m_K->setZero();

        MatrixView view([&](Dof dof_row, Dof dof_col, double val)
        {
            if(dof_row.type() == DofType::Active && dof_col.type() == DofType::Active)
                (*m_K)(dof_row.index(), dof_col.index()) += val;
        });

        for(auto e: elements)
        {
            e->update_state();  // Todo: Redundant if q has been called before
            e->get_tangent_stiffness(view);
        }
    }

    return *m_K;
}

Node System::create_node(std::array<DofType, 3> type, std::array<double, 3> u_node, std::array<double, 3> v_node)
{
    return Node({create_dof(type[0], u_node[0], v_node[0]),
                 create_dof(type[1], u_node[1], v_node[1]),
                 create_dof(type[2], u_node[2], v_node[2])});
}

Node System::create_node(const Node& other, std::array<DofType, 3> type)
{
    return Node({create_dof(type[0], other[0].u(), other[0].v()),
                 create_dof(type[1], other[1].u(), other[1].v()),
                 create_dof(type[2], other[2].u(), other[2].v())});
}

Dof System::create_dof(DofType type, double u_dof, double v_dof)
{
    switch(type)
    {
        case DofType::Active:
        {
            size_t n = u().size() + 1;
            u_mut() = (VectorXd(n) << u_mut(), u_dof).finished();
            v_mut() = (VectorXd(n) << v_mut(), v_dof).finished();
            p_mut() = (VectorXd(n) << p_mut(), 0.0).finished();

            return {*this, n-1};
        }

        case DofType::Fixed:
        {
            return {*this, u_dof};
        }
    }
}
