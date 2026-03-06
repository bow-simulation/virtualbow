#pragma once
#include "pre/widgets/PropertyList.hpp"

class HandleModel;

class HandleView: public PropertyList
{
public:
    HandleView(HandleModel* model);
};
