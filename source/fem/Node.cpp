#include "Node.hpp"
#include "System.hpp"

Dof::Dof(size_t i)
    : m_active(true),
      m_i(i)
{

}

Dof::Dof(double u)
    : m_active(false),
      m_u(u)
{

}

Dof::Dof()
{

}

size_t Dof::index() const
{
    assert(m_active);
    return m_i;
}

bool Dof::type() const
{
    return m_active;
}

bool Dof::operator!=(const Dof& rhs) const
{
    return (m_active != rhs.m_active) || (m_i != rhs.m_i);
}
