#pragma once
#include "pre/widgets/PropertyList.hpp"

class ArcVM;

class ArcView: public PropertyList
{
public:
    ArcView(ArcVM* model);
};
