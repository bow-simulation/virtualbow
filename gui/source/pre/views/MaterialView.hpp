#pragma once
#include "pre/widgets/PropertyList.hpp"

class MaterialModel;

class MaterialView: public PropertyList
{
public:
    MaterialView(MaterialModel* model);
};
