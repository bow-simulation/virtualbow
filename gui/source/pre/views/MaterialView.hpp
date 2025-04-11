#pragma once
#include "pre/widgets/PropertyList.hpp"

class MaterialVM;

class MaterialView: public PropertyList
{
public:
    MaterialView(MaterialVM* model);
};
