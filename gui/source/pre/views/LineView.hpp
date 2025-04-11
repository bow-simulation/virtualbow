#pragma once
#include "pre/widgets/PropertyList.hpp"

class LineVM;

class LineView: public PropertyList
{
public:
    LineView(LineVM* model);
};
