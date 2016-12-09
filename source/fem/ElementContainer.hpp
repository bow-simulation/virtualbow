#pragma once
#include "elements/Element.hpp"
#include "../utils/DynamicCastIterator.hpp"
#include <boost/range/iterator_range.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <string>
#include <vector>
#include <map>

class ElementContainer
{
public:
    template<class ElementType>
    using iterator = dynamic_cast_iterator<std::vector<Element*>::iterator, ElementType>;

    template<class ElementType>
    using const_iterator = dynamic_cast_iterator<std::vector<Element*>::const_iterator, ElementType>;

    template<typename ElementType = Element>
    void add(ElementType element, const std::string& key = "")
    {
        Element* ptr = new ElementType(element);
        groups[key].push_back(ptr);
        elements.push_back(ptr);
    }

    // Iterating over groups

    template<class ElementType = Element>
    boost::iterator_range<iterator<ElementType>> group(const std::string& key)
    {
        return {groups[key].begin(), groups[key].end()};
    }

    template<class ElementType = Element>
    boost::iterator_range<const_iterator<ElementType>> group(const std::string& key) const
    {
        return {groups[key].begin(), groups[key].end()};
    }

    // Single element access

    template<class ElementType = Element>
    ElementType& item(const std::string& key)
    {
        return dynamic_cast<ElementType&>(*groups[key][0]);
    }

    template<class ElementType = Element>
    const ElementType& item(const std::string& key) const
    {
        return dynamic_cast<const ElementType&>(*groups[key][0]);
    }

    // Iterate over all elements

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

private:
    mutable std::map<std::string, std::vector<Element*>> groups;
    mutable std::vector<Element*> elements;
};
