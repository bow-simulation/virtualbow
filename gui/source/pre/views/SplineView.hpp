#pragma once
#include "pre/widgets/PropertyList.hpp"

class TableModel;

class SplineView: public PropertyList
{
public:
    SplineView(TableModel* model);
};
