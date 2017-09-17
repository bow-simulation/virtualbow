#pragma once
#include "fem/Node.hpp"
#include "fem/Element.hpp"
#include "utils/DynamicCastIterator.hpp"
#include "utils/Optional.hpp"

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

// Another problem:
// Element::update_state() is called too often (in q and K).
// Perhaps abandon this whole approach for a simpler implementation that
// requires the solution algorithms to do a bit more "book keeping".

class System
{
private:
    double m_t = 0.0;  // Todo: Really necessary here?
    VectorXd m_u;   // Active displacements
    VectorXd m_v;   // Active velocities
    VectorXd m_p;   // External forces

    mutable Optional<VectorXd> m_a;    // Accelerations
    mutable Optional<VectorXd> m_q;    // Internal forces
    mutable Optional<VectorXd> m_M;    // Mass matrix (diagonal)
    mutable Optional<MatrixXd> m_K;    // Tangent stiffness matrix

    // Todo: Why mutable?
    // Todo: Use https://github.com/Tessil/ordered-map
    mutable std::map<std::string, std::vector<Element*>> groups;
    mutable std::vector<Element*> elements;

public:
    size_t dofs() const;

    double& t_mut();
    VectorXd& u_mut();
    VectorXd& v_mut();
    VectorXd& p_mut();

    const double& t() const;
    const VectorXd& u() const;
    const VectorXd& v() const;
    const VectorXd& p() const;

    // Calculated data

    const VectorXd& a() const;
    const VectorXd& q() const;
    const VectorXd& M() const;
    const MatrixXd& K() const;

    // Nodes and elements

    Node create_node(std::array<DofType, 3> type, std::array<double, 3> u_node, std::array<double, 3> v_node = {0.0, 0.0, 0.0});
    Node create_node(const Node& other, std::array<DofType, 3> type);
    Dof create_dof(DofType type, double u_dof, double v_dof);

    template<typename ElementType = Element>
    void add_element(ElementType element, const std::string& key = "")
    {
        m_a.set_valid(false);
        m_q.set_valid(false);
        m_M.set_valid(false);
        m_K.set_valid(false);

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
        m_a.set_valid(false);
        m_q.set_valid(false);
        m_M.set_valid(false);
        m_K.set_valid(false);

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
        m_a.set_valid(false);
        m_q.set_valid(false);
        m_M.set_valid(false);
        m_K.set_valid(false);

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
        VectorView get_v([&](Dof dof){ return dof.v(); }, nullptr, nullptr);  // Todo

        double e_kin = 0.0;
        for(auto e: groups[key])
        {
            e_kin += e->get_kinetic_energy(get_v);
        }

        return e_kin;
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
