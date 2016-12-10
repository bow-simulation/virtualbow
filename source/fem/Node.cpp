#include "Node.hpp"
#include "System.hpp"

Dof::Dof(System& system, size_t i)
    : m_system(&system),
      m_type(DofType::Active),
      m_i(i)
{

}

Dof::Dof(System& system, double u)
    : m_system(&system),
      m_type(DofType::Fixed),
      m_u(u)
{

}

Dof::Dof()
{

}

size_t Dof::index() const
{
    assert(m_type == DofType::Active);
    return m_i;
}

DofType Dof::type() const
{
    return m_type;
}


bool Dof::operator!=(const Dof& rhs) const
{
    return (m_system != rhs.m_system) || (m_type != rhs.m_type) || (m_i != rhs.m_i);
}

double Dof::u() const
{
    return m_type == DofType::Active ? m_system->u()(m_i) : m_u;
}

double Dof::v() const
{
    return m_type == DofType::Active ? m_system->v()(m_i) : 0.0;
}

double Dof::a() const
{
    return m_type == DofType::Active ? m_system->a()(m_i) : 0.0;
}

double Dof::p() const
{
    return m_type == DofType::Active ? m_system->p()(m_i) : 0.0;
}

double& Dof::p_mut()
{
    return m_system->p_mut()(index());
}

Node::Node(std::array<Dof, 3> dofs): dofs(dofs)
{

}

Node::Node()//: dofs({{}, {}, {}})
{

}

Dof Node::operator[](size_t i) const
{
    return dofs[i];
}

double Node::distance(const Node& node_a, const Node& node_b)
{
    double dx = node_b[0].u() - node_a[0].u();
    double dy = node_b[1].u() - node_a[1].u();

    return std::hypot(dx, dy);
}

double Node::angle(const Node& node_a, const Node& node_b)
{
    // Todo: Code duplication
    double dx = node_b[0].u() - node_a[0].u();
    double dy = node_b[1].u() - node_a[1].u();

    return std::atan2(dy, dx);
}
