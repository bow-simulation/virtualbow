#pragma once
#include "Node.hpp"
#include "Element.hpp"
#include "ElementContainer.hpp"
#include "Dependency.hpp"
#include "numerics/Math.hpp"

#include <boost/range/iterator_range.hpp>
#include <string>
#include <vector>
#include <map>

class System
{
private:
    // Independent variables
    Independent<ElementContainer> elements;

    double t;                     // Todo: Move to solver class?
    Independent<size_t> n_a;      // Number of active Dofs
    Independent<size_t> n_f;      // Number of fixed Dofs
    Independent<VectorXd> u_a;    // Displacements (active)
    Independent<VectorXd> u_f;    // Displacements (fixed)
    Independent<VectorXd> v_a;    // Velocities (active)
    Independent<VectorXd> p_a;    // External forces (active)

    // Dependent variables
    mutable Dependent<VectorXd> a_a;    // Accelerations (active)
    mutable Dependent<VectorXd> q_a;    // Internal forces (active)
    mutable Dependent<VectorXd> q_f;    // Internal forces (fixed)
    mutable Dependent<VectorXd> M_a;    // Diagonal masses (active)
    mutable Dependent<MatrixXd> K_a;    // Stiffness matrix (active)

public:
    System();

    // Node creation

    Node create_node(std::array<bool, 3> active, std::array<double, 3> u);
    Node create_node(const Node& other);
    Dof create_dof(bool active, double u);

    // Element access

    const ElementContainer& get_elements() const;
    ElementContainer& mut_elements();

    // System data access

    size_t dofs() const;

    double get_t() const;
    void set_t(double t);

    const VectorXd& get_u() const;
    void set_u(const VectorXd& u);
    double get_u(Dof dof) const;
    double get_angle(const Node& a, const Node& b);
    double get_distance(const Node& a, const Node& b);

    const VectorXd& get_v() const;
    void set_v(const VectorXd& v);
    double get_v(Dof dof) const;

    const VectorXd& get_p() const;
    void set_p(const VectorXd& p);
    double get_p(Dof dof) const;
    void set_p(Dof dof, double p);

    const VectorXd& get_a() const;
    double get_a(Dof dof) const;

    const VectorXd& get_q() const;
    double get_q(Dof dof) const;

    const VectorXd& get_M() const;
    const MatrixXd& get_K() const;


    // Set calculated values (Element interface)

    void add_q(Dof dof, double q)
    {
        add_by_dof(q_a.mut(), q_f.mut(), dof, q);
    }

    template<size_t N, class T>
    void add_q(const std::array<Dof, N>& dofs, const T& q)
    {
        add_by_dof(q_a.mut(), q_f.mut(), dofs, q);
    }

    void add_M(Dof dof, double m)
    {
        add_by_dof(M_a.mut(), dof, m);
    }

    template<size_t N, class T>
    void add_M(const std::array<Dof, N>& dofs, const T& m)
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
};
