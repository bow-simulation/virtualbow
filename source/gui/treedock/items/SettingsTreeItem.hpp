#pragma once
#include "gui/treedock/TreeItem.hpp"
#include "solver/model/input/InputData.hpp"

class ViewModel;
class IntegerPropertyItem;
class DoublePropertyItem;
class PropertyTreeWidget;

class SettingsTreeItem: public TreeItem
{
public:
    SettingsTreeItem(ViewModel* model);
    void updateModel(void* source);
    void updateView(void* source);

private:
    PropertyTreeWidget* property_tree;
    IntegerPropertyItem* n_limb_elements;
    IntegerPropertyItem* n_string_elements;
    IntegerPropertyItem* n_draw_steps;
    DoublePropertyItem* arrow_clamp_force;
    DoublePropertyItem* time_span_factor;
    DoublePropertyItem* time_step_factor;
    DoublePropertyItem* sampling_rate;
};
