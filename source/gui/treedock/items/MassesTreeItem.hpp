#pragma once
#include "gui/treedock/TreeItem.hpp"
#include "solver/model/input/InputData.hpp"

class ViewModel;
class DoublePropertyItem;
class PropertyTreeWidget;

class MassesTreeItem: public TreeItem {
public:
    MassesTreeItem(ViewModel* model);
    void updateModel(void* source) override;
    void updateView(void* source) override;

private:
    PropertyTreeWidget* property_tree;
    DoublePropertyItem* arrow;
    DoublePropertyItem* string_center;
    DoublePropertyItem* string_tip;
    DoublePropertyItem* limb_tip;
};
