#pragma once
#include <boost/iterator/transform_iterator.hpp>

// Dynamic cast iterator adapted from stackoverflow:
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
