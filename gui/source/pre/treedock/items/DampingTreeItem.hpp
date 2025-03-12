#pragma once
#include "pre/treedock/TreeItem.hpp"
#include "solver/input.hpp"

class ViewModel;
class DoublePropertyItem;
class PropertyTreeWidget;

class DampingTreeItem: public TreeItem {
public:
    DampingTreeItem(ViewModel* model);
    void updateModel(void* source);
    void updateView(void* source);

private:
    PropertyTreeWidget* property_tree;
    DoublePropertyItem* damping_ratio_limbs;
    DoublePropertyItem* damping_ratio_string;
};
