#pragma once
#include "pre/widgets/PropertyList.hpp"

class DimensionsVM;

class DimensionsView: public PropertyList
{
public:
    DimensionsView(DimensionsVM* model);
};
