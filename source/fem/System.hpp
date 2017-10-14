#pragma once
#include "numerics/Math.hpp"
#include "fem/Node.hpp"
#include "fem/Element.hpp"
#include "utils/DynamicCastIterator.hpp"
#include "utils/Invalidatable.hpp"

#include <boost/range/iterator_range.hpp>
#include <string>
#include <vector>
#include <map>

// Todo: Weakness of the current approach:
//
// VectorXd& v = system.v_mut();    // Invalidation of a
// VectorXd& a = system.a();        // Recomputation of a
// v(3) = xyz;                      // a is now technically invalid, but will not be recomputed
//
// Solution 1: Don't store references
// Solution 2: Don't return raw references, but some object Ref<T> that does the invalidation when it goes out of scope?
//
// Problem is also when it's done in one statement:
//
// system.v_mut() = system.a();
//
// 1. v_mut called, a invalidated
// 2. a recomputed
// 3. a assigned to v, a now out of date

class System
{
private:
    // Independent variables
    double t = 0.0;  // Todo: Really necessary here?
    VectorXd u_a;    // Displacements (active)
    VectorXd u_f;    // Displacements (fixed)
    VectorXd v_a;    // Velocities (active)
    VectorXd p_a;    // External forces (active)

    // Dependent variables
    mutable Invalidatable<VectorXd> a_a;    // Accelerations (active)
    mutable Invalidatable<VectorXd> q_a;    // Internal forces (active)
    mutable Invalidatable<VectorXd> q_f;    // Internal forces (fixed)
    mutable Invalidatable<VectorXd> M_a;    // Diagonal masses (active)
    mutable Invalidatable<MatrixXd> K_a;    // Stiffness matrix (active)

    // Todo: Why mutable?
    // Todo: Use https://github.com/Tessil/ordered-map
    mutable std::map<std::string, std::vector<Element*>> groups;
    mutable std::vector<Element*> elements;

public:
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
        return u_a;
    }

    double get_u(Dof dof) const
    {
        return get_u(std::array<Dof, 1>{dof})[0];
    }

    template<size_t N>
    Vector<N> get_u(const std::array<Dof, N>& dofs) const
    {
        return get_by_dof(u_a, u_f, dofs);
    }

    void set_u(const VectorXd& u)
    {
        u_a = u;
        a_a.invalidate();
        q_a.invalidate();
        K_a.invalidate();
    }

    // Get and set velocity

    const VectorXd& get_v() const
    {
        return v_a;
    }

    double get_v(Dof dof) const
    {
        return get_by_dof(v_a, dof);
    }

    template<size_t N>
    Vector<N> get_v(const std::array<Dof, N>& dofs) const
    {
        return get_by_dof(v_a, dofs);
    }

    void set_v(const VectorXd& v)
    {
        v_a = v;
        a_a.invalidate();
        q_a.invalidate();
    }

    // Get and set external forces

    const VectorXd& get_p() const
    {
        return p_a;
    }

    double get_p(Dof dof) const
    {
        dof.active ? get_p()(dof.index) : 0.0;
    }

    void set_p(const VectorXd& p)
    {
        p_a = p;
        a_a.invalidate();
    }

    void set_p(Dof dof, double p)
    {
        set_by_dof(p_a, dof, p);
        a_a.invalidate();
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
        add_by_dof(*q_a, *q_f, dof, q);
    }

    template<size_t N, class T>
    void add_q(const std::array<Dof, N>& dofs, const T& q)    // Todo: Type of q too generic
    {
        add_by_dof(*q_a, *q_f, dofs, q);
    }

    void add_M(Dof dof, double m)
    {
        add_by_dof(*M_a, dof, m);
    }

    template<size_t N, class T>
    void add_M(const std::array<Dof, N>& dofs, const T& m)    // Todo: Type of m too generic
    {
        add_by_dof(*M_a, dofs, m);
    }

    void add_K(Dof dof_row, Dof dof_col, double k)
    {
        if(dof_row.active && dof_col.active)
        {
            (*K_a)(dof_row.index, dof_col.index) += k;

            if(dof_row.index != dof_col.index)
                (*K_a)(dof_col.index, dof_row.index) += k;
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
                    (*K_a)(dofs[i].index, dofs[j].index) += k(i, j);
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










    // Old stuff below















    // Calculated data

    // Nodes and elements

public:
    Node create_node(std::array<bool, 3> active, std::array<double, 3> u);
    Node create_node(const Node& other);
    Dof create_dof(bool active, double u);

    template<typename ElementType = Element>
    void add_element(ElementType element, const std::string& key = "")
    {
        a_a.invalidate();
        q_a.invalidate();
        M_a.invalidate();
        K_a.invalidate();

        Element* ptr = new ElementType(element);
        groups[key].push_back(ptr);
        elements.push_back(ptr);
    }

    // Iterating over groups of elements

    template<class ElementType>
    using iterator = dynamic_cast_iterator<std::vector<Element*>::iterator, ElementType>;

    template<class ElementType>
    using const_iterator = dynamic_cast_iterator<std::vector<Element*>::const_iterator, ElementType>;

    template<class ElementType>
    boost::iterator_range<iterator<ElementType>> element_group_mut(const std::string& key)
    {
        a_a.invalidate();
        q_a.invalidate();
        M_a.invalidate();
        K_a.invalidate();

        return {groups[key].begin(), groups[key].end()};
    }

    template<class ElementType>
    boost::iterator_range<const_iterator<ElementType>> element_group(const std::string& key) const
    {
        return {groups[key].begin(), groups[key].end()};
    }

    // Single element access

    template<class ElementType>
    ElementType& element_mut(const std::string& key)
    {
        a_a.invalidate();
        q_a.invalidate();
        M_a.invalidate();
        K_a.invalidate();

        return dynamic_cast<ElementType&>(*groups[key][0]);
    }

    template<class ElementType>
    const ElementType& element(const std::string& key) const
    {
        return dynamic_cast<const ElementType&>(*groups[key][0]);
    }

    template<typename... Args>
    double get_kinetic_energy(const std::string& key, Args... args) const
    {
        return get_kinetic_energy(key) + get_kinetic_energy(args...);
    }

    template<typename... Args>
    double get_potential_energy(const std::string& key, Args... args) const
    {
        return get_potential_energy(key) + get_potential_energy(args...);
    }

    double get_kinetic_energy(const std::string& key) const
    {
        /*
        VectorView<Dof> get_v([&](Dof dof){ return get_v(dof); }, nullptr, nullptr);  // Todo

        double e_kin = 0.0;
        for(auto e: groups[key])
        {
            e_kin += e->get_kinetic_energy(get_v);
        }

        return e_kin;
        */

        return 0.0;
    }

    double get_potential_energy(const std::string& key) const
    {
        double e_pot = 0.0;
        for(auto e: groups[key])
        {
            e_pot += e->get_potential_energy();
        }

        return e_pot;
    }
};
