#pragma once
#include "pre/treedock/TreeItem.hpp"
#include "solver/model/input/InputData.hpp"

class ViewModel;
class DoublePropertyItem;
class PropertyTreeWidget;

class DimensionsTreeItem: public TreeItem
{
public:
    DimensionsTreeItem(ViewModel* model);
    void updateModel(void* source);
    void updateView(void* source);

private:
    PropertyTreeWidget* property_tree;
    DoublePropertyItem* brace_height;
    DoublePropertyItem* draw_length;
    DoublePropertyItem* handle_length;
    DoublePropertyItem* handle_setback;
    DoublePropertyItem* handle_angle;
};
