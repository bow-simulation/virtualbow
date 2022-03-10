#pragma once
#include "gui/treedock/TreeItem.hpp"
#include "solver/model/input/InputData.hpp"

class DataViewModel;
class DoublePropertyItem;
class IntegerPropertyItem;

class StringTreeItem: public TreeItem {
public:
    StringTreeItem(DataViewModel* model);
    void updateModel();
    void updateView();

private:
    DataViewModel* model;
    DoublePropertyItem* strand_stiffness;
    DoublePropertyItem* strand_density;
    IntegerPropertyItem* n_strands;
};
