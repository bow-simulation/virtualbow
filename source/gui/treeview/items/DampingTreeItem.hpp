#pragma once
#include "gui/treeview/TreeItem.hpp"
#include "solver/model/input/InputData.hpp"

class DataViewModel;
class DoublePropertyItem;

class DampingTreeItem: public TreeItem {
public:
    DampingTreeItem(DataViewModel* model);
    void updateModel();
    void updateView();

private:
    DataViewModel* model;
    DoublePropertyItem* damping_ratio_limbs;
    DoublePropertyItem* damping_ratio_string;
};
