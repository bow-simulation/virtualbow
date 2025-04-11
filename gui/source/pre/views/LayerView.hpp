#pragma once
#include "pre/widgets/PropertyList.hpp"

class LayerVM;
class TableModel;

class LayerView: public PropertyList
{
public:
    LayerView(LayerVM* model, TableModel* tableModel);
};
