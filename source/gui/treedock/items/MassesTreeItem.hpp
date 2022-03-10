#pragma once
#include "gui/treedock/TreeItem.hpp"
#include "solver/model/input/InputData.hpp"

class DataViewModel;
class DoublePropertyItem;

class MassesTreeItem: public TreeItem {
public:
    MassesTreeItem(DataViewModel* model);
    void updateModel();
    void updateView();

private:
    DataViewModel* model;
    DoublePropertyItem* arrow;
    DoublePropertyItem* string_center;
    DoublePropertyItem* string_tip;
    DoublePropertyItem* limb_tip;
};
