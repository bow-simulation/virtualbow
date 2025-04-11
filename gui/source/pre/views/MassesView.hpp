#pragma once
#include "pre/widgets/PropertyList.hpp"

class MassesVM;

class MassesView: public PropertyList
{
public:
    MassesView(MassesVM* model);
};
