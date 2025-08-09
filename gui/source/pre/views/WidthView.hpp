#pragma once
#include "pre/widgets/PropertyList.hpp"

class TableModel;

class WidthView: public PropertyList
{
public:
    WidthView(TableModel* model);
};
