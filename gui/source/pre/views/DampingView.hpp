#pragma once
#include "pre/widgets/PropertyList.hpp"

class DampingModel;

class DampingView: public PropertyList
{
public:
    DampingView(DampingModel* model);
};
