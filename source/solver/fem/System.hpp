#pragma once
#include "Node.hpp"
#include "DofView.hpp"
#include "Element.hpp"
#include "ElementContainer.hpp"
#include "Dependency.hpp"
#include "solver/numerics/EigenTypes.hpp"

class System
{
private:
    Independent<ElementContainer> elements;

    double t;                     // Time
    Independent<size_t> n_a;      // Number of active Dofs
    Independent<size_t> n_f;      // Number of fixed Dofs
    Independent<VectorXd> u_a;    // Displacements (active)
    Independent<VectorXd> u_f;    // Displacements (fixed)
    Independent<VectorXd> v_a;    // Velocities (active)
    Independent<VectorXd> p_a;    // External forces (active)

    mutable Dependent<VectorXd> a_a;    // Accelerations (active)
    mutable Dependent<VectorXd> q_a;    // Internal forces (active)
    mutable Dependent<VectorXd> q_f;    // Internal forces (fixed)
    mutable Dependent<VectorXd> M_a;    // Diagonal masses (active)
    mutable Dependent<MatrixXd> K_a;    // Tangent stiffness matrix (active)
    mutable Dependent<MatrixXd> D_a;    // Tangent damping matrix (active)

public:
    System();

    // Nodes and elements

    Node create_node(std::array<bool, 3> active, std::array<double, 3> u);
    Node create_node(const Node& other);
    Dof create_dof(bool active, double u, double v = 0.0);

    double get_angle(const Node& a, const Node& b);
    double get_distance(const Node& a, const Node& b);

    const ElementContainer& get_elements() const;
    ElementContainer& mut_elements();

    // System data

    size_t dofs() const;

    double get_t() const;
    void set_t(double t);

    const VectorXd& get_u() const;
    const VectorXd& get_v() const;
    const VectorXd& get_p() const;
    const VectorXd& get_a() const;
    const VectorXd& get_q() const;
    const VectorXd& get_M() const;
    const MatrixXd& get_K() const;
    const MatrixXd& get_D() const;

    double get_u(Dof dof) const;
    double get_v(Dof dof) const;
    double get_p(Dof dof) const;
    double get_a(Dof dof) const;
    double get_q(Dof dof) const;

    template<size_t N>
    Vector<N> get_u(const std::array<Dof, N>& dofs) const;
    template<size_t N>
    Vector<N> get_v(const std::array<Dof, N>& dofs) const;
    template<size_t N>
    Vector<N> get_p(const std::array<Dof, N>& dofs) const;
    template<size_t N>
    Vector<N> get_a(const std::array<Dof, N>& dofs) const;
    template<size_t N>
    Vector<N> get_q(const std::array<Dof, N>& dofs) const;

    void set_u(const Ref<const VectorXd>& u);
    void set_v(const Ref<const VectorXd>& v);
    void set_p(const Ref<const VectorXd>& p);
    void set_p(Dof dof, double p);

    template<size_t N, class T>
    void add_q(const std::array<Dof, N>& dofs, const T& q);
    template<size_t N, class T>
    void add_M(const std::array<Dof, N>& dofs, const T& M);
    template<size_t N, class T>
    void add_K(const std::array<Dof, N>& dofs, const T& K);
    template<size_t N, class T>
    void add_D(const std::array<Dof, N>& dofs, const T& D);
};

template<size_t N>
Vector<N> System::get_u(const std::array<Dof, N>& dofs) const
{
    Vector<N> result;
    for(size_t i = 0; i < N; ++i) {
        result(i) = get_u(dofs[i]);
    }

    return result;
}

template<size_t N>
Vector<N> System::get_v(const std::array<Dof, N>& dofs) const
{
    Vector<N> result;
    for(size_t i = 0; i < N; ++i) {
        result(i) = get_v(dofs[i]);
    }

    return result;
}

template<size_t N>
Vector<N> System::get_p(const std::array<Dof, N>& dofs) const
{
    Vector<N> result;
    for(size_t i = 0; i < N; ++i) {
        result(i) = get_p(dofs[i]);
    }

    return result;
}

template<size_t N>
Vector<N> System::get_a(const std::array<Dof, N>& dofs) const
{
    Vector<N> result;
    for(size_t i = 0; i < N; ++i) {
        result(i) = get_a(dofs[i]);
    }

    return result;
}

template<size_t N>
Vector<N> System::get_q(const std::array<Dof, N>& dofs) const
{
    Vector<N> result;
    for(size_t i = 0; i < N; ++i) {
        result(i) = get_q(dofs[i]);
    }

    return result;
}

template<size_t N, class T>
void System::add_q(const std::array<Dof, N>& dofs, const T& q)
{
    add_by_dof(&q_a.mut(), &q_f.mut(), dofs, q);
}

template<size_t N, class T>
void System::add_M(const std::array<Dof, N>& dofs, const T& M)
{
    add_by_dof(&M_a.mut(), nullptr, dofs, M);
}

template<size_t N, class T>
void System::add_K(const std::array<Dof, N>& dofs, const T& K)
{
    add_by_dof(&K_a.mut(), dofs, K);
}

template<size_t N, class T>
void System::add_D(const std::array<Dof, N>& dofs, const T& D)
{
    add_by_dof(&D_a.mut(), dofs, D);
}
