#pragma once
#include "gui/treeview/TreeItem.hpp"
#include "solver/model/input/InputData.hpp"

class DataViewModel;
class DoublePropertyItem;

class DimensionsTreeItem: public TreeItem
{
public:
    DimensionsTreeItem(DataViewModel* model);
    void updateModel();
    void updateView();

private:
    DataViewModel* model;
    DoublePropertyItem* brace_height;
    DoublePropertyItem* draw_length;
    DoublePropertyItem* handle_length;
    DoublePropertyItem* handle_setback;
    DoublePropertyItem* handle_angle;
};
