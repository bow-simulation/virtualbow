#pragma once
#include "pre/treedock/TreeItem.hpp"
#include "solver/model/input/InputData.hpp"

class ViewModel;
class DoublePropertyItem;
class IntegerPropertyItem;
class PropertyTreeWidget;

class StringTreeItem: public TreeItem {
public:
    StringTreeItem(ViewModel* model);
    void updateModel(void* source);
    void updateView(void* source);

private:
    PropertyTreeWidget* property_tree;
    DoublePropertyItem* strand_stiffness;
    DoublePropertyItem* strand_density;
    IntegerPropertyItem* n_strands;
};
