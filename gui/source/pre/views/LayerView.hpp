#pragma once
#include "pre/widgets/PropertyList.hpp"

class LayerModel;
class TableModel;

class LayerView: public PropertyList
{
public:
    LayerView(LayerModel* model, TableModel* tableModel);
};
