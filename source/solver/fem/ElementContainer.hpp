#pragma once
#include "Element.hpp"
#include <boost/range/iterator_range.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <vector>
#include <map>

// Transform iterator that performs a dynamic cast for every element. Adapted from stackoverflow,
// http://stackoverflow.com/questions/842387/how-do-i-dynamically-cast-between-vectors-of-pointers/842491#842491

template<class Target>
struct dynamic_caster
{
    typedef Target& result_type;

    template<typename Source>
    result_type& operator()(Source* s) const
    {
         return dynamic_cast<result_type&>(*s);
    }
};

template<class Iterator, class Target>
using dynamic_cast_iterator_base = boost::transform_iterator<dynamic_caster<Target>, Iterator>;

template<class Iterator, class Target>
class dynamic_cast_iterator: public dynamic_cast_iterator_base<Iterator, Target>
{
public:
    dynamic_cast_iterator(Iterator it)
        : dynamic_cast_iterator_base<Iterator, Target>(it, dynamic_caster<Target>())
    {

    }
};

// The element container stores groups of elements that are identified by keys (string)

class ElementContainer
{
public:
    template<typename ElementType = Element>
    void add(ElementType element, const std::string& key = "")
    {
        Element* ptr = new ElementType(element);
        groups[key].push_back(ptr);
        elements.push_back(ptr);
    }

    ~ElementContainer()
    {
        for(Element* e: elements)
            delete e;
    }

    template<class ElementType>
    using iterator = dynamic_cast_iterator<std::vector<Element*>::iterator, ElementType>;

    template<class ElementType>
    using const_iterator = dynamic_cast_iterator<std::vector<Element*>::const_iterator, ElementType>;

    // Iterating over all elements

    iterator<Element> begin()
    {
        return {elements.begin()};
    }

    const_iterator<Element> begin() const
    {
        return {elements.begin()};
    }

    iterator<Element> end()
    {
        return {elements.end()};
    }

    const_iterator<Element> end() const
    {
        return {elements.end()};
    }

    // Iterating over groups of elements

    template<class ElementType>
    boost::iterator_range<iterator<ElementType>> group(const std::string& key)
    {
        return {groups[key].begin(), groups[key].end()};
    }

    template<class ElementType>
    boost::iterator_range<const_iterator<ElementType>> group(const std::string& key) const
    {
        return {groups[key].begin(), groups[key].end()};
    }

    // Single element access

    template<class ElementType>
    ElementType& front(const std::string& key)
    {
        return dynamic_cast<ElementType&>(*groups[key].front());
    }

    template<class ElementType>
    const ElementType& front(const std::string& key) const
    {
        return dynamic_cast<const ElementType&>(*groups[key].front());
    }

    template<class ElementType>
    ElementType& back(const std::string& key)
    {
        return dynamic_cast<ElementType&>(*groups[key].back());
    }

    template<class ElementType>
    const ElementType& back(const std::string& key) const
    {
        return dynamic_cast<const ElementType&>(*groups[key].back());
    }

    // Summing energies of groups

    double get_kinetic_energy(const std::string& key) const
    {
        double e_kin = 0.0;
        for(auto e: groups[key])
            e_kin += e->get_kinetic_energy();

        return e_kin;
    }

    double get_potential_energy(const std::string& key) const
    {
        double e_pot = 0.0;
        for(auto e: groups[key])
            e_pot += e->get_potential_energy();

        return e_pot;
    }

private:
    mutable std::vector<Element*> elements;
    mutable std::map<std::string, std::vector<Element*>> groups;
};
