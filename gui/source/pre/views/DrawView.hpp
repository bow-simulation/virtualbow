#pragma once
#include "pre/widgets/PropertyList.hpp"

class DrawModel;

class DrawView: public PropertyList
{
public:
    DrawView(DrawModel* model);
};
