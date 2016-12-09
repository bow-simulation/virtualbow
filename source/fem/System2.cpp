#include "System2.hpp"

size_t System2::dofs() const
{
    return u().size();
}

double& System2::t_mut()
{
    return m_t;
}

VectorXd& System2::u_mut()
{
    m_a.set_valid(false);
    m_q.set_valid(false);
    m_K.set_valid(false);

    return m_u;
}

VectorXd& System2::v_mut()
{
    m_a.set_valid(false);
    m_q.set_valid(false);

    return m_v;
}

VectorXd& System2::p_mut()
{
    m_a.set_valid(false);

    return m_p;
}

const double& System2::t() const
{
    return m_t;
}

const VectorXd& System2::u() const
{
    return m_u;
}


const VectorXd& System2::v() const
{
    return m_v;
}

const VectorXd& System2::p() const
{
    return m_p;
}

const VectorXd& System2::a() const
{
    if(!m_a.is_valid())
    {
        m_a.set_valid(true);
        *m_a = M().asDiagonal().inverse()*(p() - q());
    }

    return *m_a;
}

const VectorXd& System2::q() const
{
    if(!m_q.is_valid())
    {
        m_q.set_valid(true);
        m_q->conservativeResize(dofs());
        m_q->setZero();

        VectorView<Dof2> view(nullptr, nullptr,
        [&](Dof2 dof, double val)
        {
            if(dof.type == DofType::Active)
                (*m_q)(dof.index) += val;
        });

        for(auto e: elements)
        {
            e->get_internal_forces(view);
        }
    }

    return *m_q;
}

const VectorXd& System2::M() const
{
    if(!m_M.is_valid())
    {
        m_M.set_valid(true);
        m_M->conservativeResize(dofs());
        m_M->setZero();

        VectorView<Dof2> view(nullptr, nullptr,
        [&](Dof2 dof, double val)
        {
            if(dof.type == DofType::Active)
                (*m_M)(dof.index) += val;
        });

        for(auto e: elements)
        {
            e->get_masses(view);
        }
    }

    return *m_M;
}

const MatrixXd& System2::K() const
{
    if(!m_K.is_valid())
    {
        m_K.set_valid(true);
        m_K->conservativeResize(dofs(), dofs());
        m_K->setZero();

        MatrixView<Dof2> view([&](Dof2 dof_row, Dof2 dof_col, double val)
        {
            if(dof_row.type == DofType::Active && dof_col.type == DofType::Active)
                (*m_K)(dof_row.index, dof_col.index) += val;
        });

        for(auto e: elements)
        {
            e->get_tangent_stiffness(view);
        }
    }

    return *m_K;
}

Node2 System2::create_node(std::array<DofType, 3> type, std::array<double, 3> u_node, std::array<double, 3> v_node)
{
    return Node2(*this, {create_dof(type[0], u_node[0], v_node[0]),
                         create_dof(type[1], u_node[1], v_node[1]),
                         create_dof(type[2], u_node[2], v_node[2])});
}

/*
Node2 System2::create_node(const Node2& other, std::array<DofType, 3> type)
{
    return Node2(*this, {create_dof(type[0], other.u(0), other.v(0)),
                         create_dof(type[1], other.u(1), other.v(1)),
                         create_dof(type[2], other.u(2), other.v(2))});
}
*/

Dof2 System2::create_dof(DofType type, double u_dof, double v_dof)
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
