#pragma once
#include "pre/widgets/PropertyList.hpp"

class MassesModel;

class MassesView: public PropertyList
{
public:
    MassesView(MassesModel* model);
};
