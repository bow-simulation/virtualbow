#pragma once
#include "pre/widgets/PropertyList.hpp"

class StringVM;

class StringView: public PropertyList
{
public:
    StringView(StringVM* model);
};
