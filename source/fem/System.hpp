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
    // Independent quantities
    double m_t = 0.0;  // Todo: Really necessary here?
    VectorXd m_u;   // Active displacements
    VectorXd m_v;   // Active velocities
    VectorXd m_p;   // External forces

    // Computed quantities
    mutable Invalidatable<VectorXd> m_a;    // Accelerations
    mutable Invalidatable<VectorXd> m_q;    // Internal forces
    mutable Invalidatable<VectorXd> m_M;    // Mass matrix (diagonal)
    mutable Invalidatable<MatrixXd> m_K;    // Tangent stiffness matrix

    // Todo: Why mutable?
    // Todo: Use https://github.com/Tessil/ordered-map
    mutable std::map<std::string, std::vector<Element*>> groups;
    mutable std::vector<Element*> elements;

public:
    size_t dofs() const;

    double get_t() const
    {
        return m_t;
    }

    void set_t(double t)
    {
        m_t = t;
    }

    // Get and set displacements

    const VectorXd& get_u() const
    {
        return m_u;
    }

    double get_u(Dof dof) const
    {
        return get_u(std::array<Dof, 1>{dof})[0];
    }

    template<size_t N>
    Vector<N> get_u(const std::array<Dof, N>& dofs) const
    {
        Vector<N> u;
        for(size_t i = 0; i < N; ++i)
            u[i] = dofs[i].m_active ? get_u()(dofs[i].m_i) : dofs[i].m_u;

        return u;
    }

    void set_u(const VectorXd& u)
    {
        m_u = u;
        m_a.invalidate();
        m_q.invalidate();
        m_K.invalidate();
    }

    // Get and set velocity

    const VectorXd& get_v() const
    {
        return m_v;
    }

    double get_v(Dof dof) const
    {
        return get_v(std::array<Dof, 1>{dof})[0];
    }

    template<size_t N>
    Vector<N> get_v(const std::array<Dof, N>& dofs) const
    {
        Vector<N> v;
        for(size_t i = 0; i < N; ++i)
            v[i] = dofs[i].m_active ? get_v()(dofs[i].m_i) : 0.0;

        return v;
    }

    void set_v(const VectorXd& v)
    {
        m_v = v;
        m_a.invalidate();
        m_q.invalidate();
    }

    // Get and set external forces

    const VectorXd& get_p() const
    {
        return m_p;
    }

    double get_p(Dof dof) const
    {
        dof.m_active ? get_p()(dof.m_i) : 0.0;
    }

    void set_p(const VectorXd& p)
    {
        m_p = p;
        m_a.invalidate();
    }

    void set_p(Dof dof, double p)
    {
        if(dof.m_active)
            m_p(dof.m_i) = p;

        m_a.invalidate();
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
        return dof.m_active ? get_a()(dof.m_i) : 0.0;
    }

    const VectorXd& get_q() const;
    const VectorXd& get_M() const;
    const MatrixXd& get_K() const;

    // Set calculated values (Element interface)

    void add_q(Dof dof, double q)
    {
        if(dof.type())
            (*m_q)(dof.index()) += q;
    }

    template<size_t N, class T>
    void add_q(const std::array<Dof, N>& dofs, const T& q)    // Todo: Type of q too generic
    {
        for(size_t i = 0; i < N; ++i)
        {
            if(dofs[i].type())
                (*m_q)(dofs[i].index()) += q[i];
        }
    }

    void add_M(Dof dof, double m)
    {
        if(dof.type())
            (*m_M)(dof.index()) += m;
    }

    template<size_t N, class T>
    void add_M(const std::array<Dof, N>& dofs, const T& m)    // Todo: Type of m too generic
    {
        for(size_t i = 0; i < N; ++i)
            add_M(dofs[i], m[i]);
    }

    void add_K(Dof dof_row, Dof dof_col, double k)
    {
        if(dof_row.type() && dof_col.type())
        {
            (*m_K)(dof_row.index(), dof_col.index()) += k;

            if(dof_row.index() != dof_col.index())
                (*m_K)(dof_col.index(), dof_row.index()) += k;
        }
    }

    template<size_t N, class T>
    void add_K(const std::array<Dof, N>& dofs, const T& k)    // Todo: Type of k too generic
    {
        for(size_t i = 0; i < N; ++i)
        {
            for(size_t j = 0; j < N; ++j)
            {
                if(dofs[i].type() && dofs[j].type())
                    (*m_K)(dofs[i].index(), dofs[j].index()) += k(i, j);
            }
        }
    }




    // Old stuff below















    // Calculated data

    // Nodes and elements

    Node create_node(std::array<bool, 3> active, std::array<double, 3> u_node, std::array<double, 3> v_node = {0.0, 0.0, 0.0});
    Node create_node(const Node& other);
    Dof create_dof(bool active, double u_dof, double v_dof);

    template<typename ElementType = Element>
    void add_element(ElementType element, const std::string& key = "")
    {
        m_a.invalidate();
        m_q.invalidate();
        m_M.invalidate();
        m_K.invalidate();

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
        m_a.invalidate();
        m_q.invalidate();
        m_M.invalidate();
        m_K.invalidate();

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
        m_a.invalidate();
        m_q.invalidate();
        m_M.invalidate();
        m_K.invalidate();

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
