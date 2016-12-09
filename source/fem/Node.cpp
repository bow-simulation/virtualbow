#include "Node.hpp"
#include "System.hpp"

Dof::Dof(System& system, size_t index)
    : system(&system),
      type(DofType::Active),
      index(index)
{

}

Dof::Dof(System& system, double u)
    : system(&system),
      type(DofType::Fixed),
      u_fixed(u)
{

}

Dof::Dof()
{

}


bool Dof::operator!=(const Dof& rhs) const
{
    return (system != rhs.system) || (type != rhs.type) || (index != rhs.index);
}

double Dof::u() const
{
    return type == DofType::Active ? system->u()(index) : u_fixed;
}

double Dof::v() const
{
    return type == DofType::Active ? system->v()(index) : 0.0;
}

double Dof::a() const
{
    return type == DofType::Active ? system->a()(index) : 0.0;
}

double Dof::p() const
{
    return type == DofType::Active ? system->p()(index) : 0.0;
}

double& Dof::p_mut()
{
    if(type == DofType::Active)
        return system->p_mut()(index);
    else
        throw std::runtime_error("Cannot modify force on fixed DOF");
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
