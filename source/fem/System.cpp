#include "System.hpp"

size_t System::dofs() const
{
    return n_a.get();
}

const VectorXd& System::get_a() const
{
    if(!a_a.is_valid())
        a_a.mut() = get_M().asDiagonal().inverse()*(get_p() - get_q());

    return a_a.get();
}

const VectorXd& System::get_q() const
{
    if(!q_a.is_valid())
    {
        q_a.mut().conservativeResize(u_a.get().size());
        q_f.mut().conservativeResize(u_f.get().size());
        q_a.mut().setZero();
        q_f.mut().setZero();

        for(auto e: elements.get())
            e->add_internal_forces();
    }

    return q_a.get();
}

const VectorXd& System::get_M() const
{
    if(!M_a.is_valid())
    {
        M_a.mut().conservativeResize(dofs());
        M_a.mut().setZero();

        for(auto e: elements.get())
            e->add_masses();
    }

    return M_a.get();
}

const MatrixXd& System::get_K() const
{
    if(!K_a.is_valid())
    {
        K_a.mut().conservativeResize(dofs(), dofs());
        K_a.mut().setZero();

        for(auto e: elements.get())
            e->add_tangent_stiffness();
    }

    return K_a.get();
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
    if(active)
    {
        n_a.mut() += 1;
        u_a.mut() = (VectorXd(n_a.get()) << u_a.get(), u).finished();
        v_a.mut() = (VectorXd(n_a.get()) << v_a.get(), 0).finished();
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
