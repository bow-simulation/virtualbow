#pragma once
#include "pre/widgets/PropertyList.hpp"

class ArcModel;

class ArcView: public PropertyList
{
public:
    ArcView(ArcModel* model);
};
