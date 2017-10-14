#pragma once
#include "Node.hpp"
#include "Element.hpp"
#include "ElementContainer.hpp"
#include "Dependent.hpp"
#include "numerics/Math.hpp"

#include <boost/range/iterator_range.hpp>
#include <string>
#include <vector>
#include <map>

class System
{
private:
    // Independent variables
    double t;                   // Todo: Move to solver class?
    Dependent<size_t> n_a;      // Number of active Dofs
    Dependent<size_t> n_f;      // Number of fixed Dofs
    Dependent<VectorXd> u_a;    // Displacements (active)
    Dependent<VectorXd> u_f;    // Displacements (fixed)
    Dependent<VectorXd> v_a;    // Velocities (active)
    Dependent<VectorXd> p_a;    // External forces (active)

    // Dependent variables
    mutable Dependent<VectorXd> a_a;    // Accelerations (active)
    mutable Dependent<VectorXd> q_a;    // Internal forces (active)
    mutable Dependent<VectorXd> q_f;    // Internal forces (fixed)
    mutable Dependent<VectorXd> M_a;    // Diagonal masses (active)
    mutable Dependent<MatrixXd> K_a;    // Stiffness matrix (active)

    mutable Dependent<ElementContainer> elements;

public:
    System()
        : t(0.0), n_a(0), n_f(0),
          u_a(VectorXd()),
          u_f(VectorXd()),
          v_a(VectorXd()),
          p_a(VectorXd())
    {
        a_a.depends_on(n_a);
        a_a.depends_on(M_a);
        a_a.depends_on(p_a);
        a_a.depends_on(q_a);

        q_a.depends_on(elements);
        q_a.depends_on(n_a);
        q_a.depends_on(u_a);
        q_a.depends_on(u_f);
        q_a.depends_on(v_a);

        q_f.depends_on(elements);
        q_f.depends_on(n_f);
        q_f.depends_on(u_a);
        q_f.depends_on(u_f);
        q_f.depends_on(v_a);

        M_a.depends_on(elements);
        M_a.depends_on(n_a);

        K_a.depends_on(elements);
        K_a.depends_on(n_a);
        K_a.depends_on(u_a);
    }

    size_t dofs() const;

    double get_t() const
    {
        return t;
    }

    void set_t(double t)
    {
        this->t = t;
    }

    // Get and set displacements

    const VectorXd& get_u() const
    {
        return u_a.get();
    }

    double get_u(Dof dof) const
    {
        return get_u(std::array<Dof, 1>{dof})[0];
    }

    template<size_t N>
    Vector<N> get_u(const std::array<Dof, N>& dofs) const
    {
        return get_by_dof(u_a.get(), u_f.get(), dofs);
    }

    void set_u(const VectorXd& u)
    {
        u_a.mut() = u;
    }

    // Get and set velocity

    const VectorXd& get_v() const
    {
        return v_a.get();
    }

    double get_v(Dof dof) const
    {
        return get_by_dof(v_a.get(), dof);
    }

    template<size_t N>
    Vector<N> get_v(const std::array<Dof, N>& dofs) const
    {
        return get_by_dof(v_a, dofs);
    }

    void set_v(const VectorXd& v)
    {
        v_a.mut() = v;
    }

    // Get and set external forces

    const VectorXd& get_p() const
    {
        return p_a.get();
    }

    double get_p(Dof dof) const
    {
        dof.active ? get_p()(dof.index) : 0.0;
    }

    void set_p(const VectorXd& p)
    {
        p_a.mut() = p;
    }

    void set_p(Dof dof, double p)
    {
        set_by_dof(p_a.mut(), dof, p);
    }

    // Helper functions

    double distance(const Node& a, const Node& b)
    {
        return std::hypot(get_u(b.x) - get_u(a.x), get_u(b.y) - get_u(a.y));
    }

    double angle(const Node& a, const Node& b)
    {
        return std::atan2(get_u(b.y) - get_u(a.y), get_u(b.x) - get_u(a.x));
    }

    // Get calculated values

    const VectorXd& get_a() const;

    double get_a(Dof dof) const
    {
        return get_by_dof(get_a(), dof);
    }

    const VectorXd& get_q() const;
    const VectorXd& get_M() const;
    const MatrixXd& get_K() const;

    // Set calculated values (Element interface)

    void add_q(Dof dof, double q)
    {
        add_by_dof(q_a.mut(), q_f.mut(), dof, q);    // Todo: One unneccessary call to mut()
    }

    template<size_t N, class T>
    void add_q(const std::array<Dof, N>& dofs, const T& q)    // Todo: Type of q too generic
    {
        add_by_dof(q_a.mut(), q_f.mut(), dofs, q);    // Todo: One unneccessary call to mut()
    }

    void add_M(Dof dof, double m)
    {
        add_by_dof(M_a.mut(), dof, m);
    }

    template<size_t N, class T>
    void add_M(const std::array<Dof, N>& dofs, const T& m)    // Todo: Type of m too generic
    {
        add_by_dof(M_a.mut(), dofs, m);
    }

    void add_K(Dof dof_row, Dof dof_col, double k)
    {
        if(dof_row.active && dof_col.active)
        {
            K_a.mut()(dof_row.index, dof_col.index) += k;

            if(dof_row.index != dof_col.index)
                K_a.mut()(dof_col.index, dof_row.index) += k;
        }
    }

    template<size_t N, class T>
    void add_K(const std::array<Dof, N>& dofs, const T& k)    // Todo: Type of k too generic
    {
        for(size_t i = 0; i < N; ++i)
        {
            for(size_t j = 0; j < N; ++j)
            {
                if(dofs[i].active && dofs[j].active)
                    K_a.mut()(dofs[i].index, dofs[j].index) += k(i, j);
            }
        }
    }

    // Element access

    const ElementContainer& get_elements() const
    {
        return elements.get();
    }

    ElementContainer& mut_elements()
    {
        return elements.mut();
    }

private:
    // General access by Dofs

    double get_by_dof(const VectorXd& vec_a, const VectorXd& vec_f, Dof dof) const
    {
        if(dof.active)
            return vec_a[dof.index];
        else
            return vec_f[dof.index];
    }

    double get_by_dof(const VectorXd& vec_a, Dof dof) const
    {
        if(dof.active)
            return vec_a[dof.index];
        else
            return 0.0;
    }

    template<size_t N>
    Vector<N> get_by_dof(const VectorXd& vec_a, const VectorXd& vec_f, const std::array<Dof, N> dofs) const
    {
        Vector<N> vec;
        for(size_t i = 0; i < N; ++i)
            vec[i] = get_by_dof(vec_a, vec_f, dofs[i]);

        return vec;
    }

    template<size_t N>
    Vector<N> get_by_dof(const VectorXd& vec_a, const std::array<Dof, N> dofs) const
    {
        Vector<N> vec;
        for(size_t i = 0; i < N; ++i)
            vec[i] = get_by_dof(vec_a, dofs[i]);

        return vec;
    }

    void set_by_dof(VectorXd& vec_a, VectorXd& vec_f, Dof dof, double value)
    {
        if(dof.active)
            vec_a[dof.index] = value;
        else
            vec_f[dof.index] = value;
    }

    void set_by_dof(VectorXd& vec_a, Dof dof, double value)
    {
        if(dof.active)
            vec_a[dof.index] = value;
    }

    template<size_t N, class T>
    void set_by_dof(VectorXd& vec_a, VectorXd& vec_f, const std::array<Dof, N> dofs, const T& values)
    {
        for(size_t i = 0; i < N; ++i)
            set_by_dof(vec_a, vec_f, dofs[i], values[i]);
    }

    void add_by_dof(VectorXd& vec_a, VectorXd& vec_f, Dof dof, double value)
    {
        if(dof.active)
            vec_a[dof.index] += value;
        else
            vec_f[dof.index] += value;
    }

    void add_by_dof(VectorXd& vec_a, Dof dof, double value)
    {
        if(dof.active)
            vec_a[dof.index] += value;
    }

    template<size_t N, class T>
    void add_by_dof(VectorXd& vec_a, VectorXd& vec_f, const std::array<Dof, N> dofs, const T& values)
    {
        for(size_t i = 0; i < N; ++i)
            add_by_dof(vec_a, vec_f, dofs[i], values[i]);
    }



public:
    Node create_node(std::array<bool, 3> active, std::array<double, 3> u);
    Node create_node(const Node& other);
    Dof create_dof(bool active, double u);
};
