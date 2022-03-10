#pragma once
#include "gui/treedock/TreeItem.hpp"
#include "solver/model/input/InputData.hpp"

class DataViewModel;

class WidthTreeItem: public TreeItem {
public:
    WidthTreeItem(DataViewModel* model);
    void updateModel();
    void updateEditor();
    void updatePlot();

private:
    DataViewModel* model;
};
