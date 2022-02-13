#pragma once
#include "gui/treeview/TreeItem.hpp"
#include "solver/model/input/InputData.hpp"

class DataViewModel;

class WidthTreeItem: public TreeItem {
public:
    WidthTreeItem(DataViewModel* model);
    void updateModel();
    void updateView();

private:
    DataViewModel* model;
};
