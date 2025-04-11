#pragma once
#include "pre/widgets/PropertyList.hpp"

class DampingVM;

class DampingView: public PropertyList
{
public:
    DampingView(DampingVM* model);
};
