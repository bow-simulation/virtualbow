#pragma once
#include "pre/widgets/PropertyList.hpp"

class SpiralModel;

class SpiralView: public PropertyList
{
public:
    SpiralView(SpiralModel* model);
};
