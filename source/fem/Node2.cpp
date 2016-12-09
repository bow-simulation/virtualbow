#include "Node2.hpp"
#include "System2.hpp"

Dof2::Dof2(System2& system, size_t index)
    : system(system),
      type(DofType::Active),
      index(index)
{

}

Dof2::Dof2(System2& system, double u)
    : system(system),
      type(DofType::Fixed),
      u_fixed(u)
{

}

bool Dof2::operator!=(const Dof2& rhs) const
{
    return (type != rhs.type) || (index != rhs.index);
}

double Dof2::u() const
{
    return type == DofType::Active ? system.u()(index) : u_fixed;
}

double Dof2::v() const
{
    return type == DofType::Active ? system.v()(index) : 0.0;
}

double Dof2::a() const
{
    return type == DofType::Active ? system.a()(index) : 0.0;
}

double Dof2::p() const
{
    return type == DofType::Active ? system.p()(index) : 0.0;
}

double& Dof2::p()
{
    if(type == DofType::Active)
        return system.p_mut()(index);
    else
        throw std::runtime_error("Cannot modify force on fixed DOF");
}



Node2::Node2(System2& system, std::array<Dof2, 3> dofs)
    : system(system), dofs(dofs)
{

}

Dof2 Node2::operator[](size_t i) const
{
    return dofs[i];
}

double Node2::distance(const Node2& node_a, const Node2& node_b)
{
    double dx = node_b[0].u() - node_a[0].u();
    double dy = node_b[1].u() - node_a[1].u();

    return std::hypot(dx, dy);
}

double Node2::angle(const Node2& node_a, const Node2& node_b)
{
    // Todo: Code duplication
    double dx = node_b[0].u() - node_a[0].u();
    double dy = node_b[1].u() - node_a[1].u();

    return std::atan2(dy, dx);
}
