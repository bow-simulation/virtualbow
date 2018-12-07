#include "System.hpp"

System::System()
    : t(0.0), n_a(0), n_f(0)
{
    auto update_a = [&]()
    {
        a_a.mut() = M_a.get().asDiagonal().inverse()*(p_a.get() - q_a.get());
    };

    auto update_q = [&]()
    {
        q_a.mut().conservativeResize(u_a.get().size());
        q_f.mut().conservativeResize(u_f.get().size());
        q_a.mut().setZero();
        q_f.mut().setZero();

        for(auto& e: elements.get())
            e.add_internal_forces();

        q_a.set_valid(true);
        q_f.set_valid(true);
    };

    auto update_M = [&]()
    {
        M_a.mut().conservativeResize(dofs());
        M_a.mut().setZero();

        for(auto& e: elements.get())
            e.add_masses();
    };

    auto update_K = [&]()
    {
        K_a.mut().conservativeResize(dofs(), dofs());
        K_a.mut().setZero();

        for(auto& e: elements.get())
            e.add_tangent_stiffness();
    };

    a_a.depends_on(M_a, p_a, q_a);
    a_a.on_update(update_a);

    q_a.depends_on(elements, n_a, u_a, u_f, v_a);
    q_a.on_update(update_q);

    q_f.depends_on(elements, n_f, u_a, u_f, v_a);
    q_f.on_update(update_q);

    M_a.depends_on(elements, n_a);
    M_a.on_update(update_M);

    K_a.depends_on(elements, n_a, u_a);
    K_a.on_update(update_K);
}

Node System::create_node(std::array<bool, 3> active, std::array<double, 3> u)
{
    return Node({create_dof(active[0], u[0]),
                 create_dof(active[1], u[1]),
                 create_dof(active[2], u[2])});
}

Node System::create_node(const Node& other)
{
    return {create_dof(other.x.active  , get_u(other.x  ), get_v(other.x  )),
            create_dof(other.y.active  , get_u(other.y  ), get_v(other.y  )),
            create_dof(other.phi.active, get_u(other.phi), get_v(other.phi))};
}

Dof System::create_dof(bool active, double u, double v)
{
    if(active)
    {
        n_a.mut() += 1;
        u_a.mut() = (VectorXd(n_a.get()) << u_a.get(), u).finished();
        v_a.mut() = (VectorXd(n_a.get()) << v_a.get(), v).finished();
        p_a.mut() = (VectorXd(n_a.get()) << p_a.get(), 0).finished();
        return Dof{active, n_a.get()-1};
    }
    else
    {
        n_f.mut() += 1;
        u_f.mut() = (VectorXd(n_f.get()) << u_f.get(), u).finished();
        return Dof{active, n_f.get()-1};
    }
}

double System::get_angle(const Node& a, const Node& b)
{
    return std::atan2(get_u(b.y) - get_u(a.y), get_u(b.x) - get_u(a.x));
}

double System::get_distance(const Node& a, const Node& b)
{
    return std::hypot(get_u(b.x) - get_u(a.x), get_u(b.y) - get_u(a.y));
}

const ElementContainer& System::get_elements() const
{
    return elements.get();
}

ElementContainer& System::mut_elements()
{
    return elements.mut();
}

size_t System::dofs() const
{
    return n_a.get();
}

double System::get_t() const
{
    return t;
}

void System::set_t(double t)
{
    this->t = t;
}

const VectorXd& System::get_u() const
{
    return u_a.get();
}

const VectorXd& System::get_v() const
{
    return v_a.get();
}

const VectorXd& System::get_p() const
{
    return p_a.get();
}

const VectorXd& System::get_a() const
{
    return a_a.get();
}

const VectorXd& System::get_q() const
{
    return q_a.get();
}

const VectorXd& System::get_M() const
{
    return M_a.get();
}

const MatrixXd& System::get_K() const
{
    return K_a.get();
}


double System::get_u(Dof dof) const
{
    return get_by_dof(&u_a.get(), &u_f.get(), dof);
}

double System::get_v(Dof dof) const
{
    return get_by_dof(&v_a.get(), nullptr, dof);
}

double System::get_p(Dof dof) const
{
    return get_by_dof(&p_a.get(), nullptr, dof);
}

double System::get_a(Dof dof) const
{
    return get_by_dof(&a_a.get(), nullptr, dof);
}

double System::get_q(Dof dof) const
{
    return get_by_dof(&q_a.get(), &q_f.get(), dof);
}

void System::set_u(const Ref<const VectorXd>& u)
{
    u_a.mut() = u;
}

void System::set_v(const Ref<const VectorXd>& v)
{
    v_a.mut() = v;
}

void System::set_p(const Ref<const VectorXd>& p)
{
    p_a.mut() = p;
}

void System::set_p(Dof dof, double p)
{
    set_by_dof(&p_a.mut(), nullptr, dof, p);
}
