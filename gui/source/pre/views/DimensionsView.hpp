#pragma once
#include "pre/widgets/PropertyList.hpp"

class DimensionsModel;

class DimensionsView: public PropertyList
{
public:
    DimensionsView(DimensionsModel* model);
};
