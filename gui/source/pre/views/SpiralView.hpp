#pragma once
#include "pre/widgets/PropertyList.hpp"

class SpiralVM;

class SpiralView: public PropertyList
{
public:
    SpiralView(SpiralVM* model);
};
