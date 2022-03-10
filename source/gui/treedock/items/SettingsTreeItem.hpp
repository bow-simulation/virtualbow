#pragma once
#include "gui/treedock/TreeItem.hpp"
#include "solver/model/input/InputData.hpp"

class DataViewModel;
class IntegerPropertyItem;
class DoublePropertyItem;

class SettingsTreeItem: public TreeItem
{
public:
    SettingsTreeItem(DataViewModel* model);
    void updateModel();
    void updateView();

private:
    DataViewModel* model;
    IntegerPropertyItem* n_limb_elements;
    IntegerPropertyItem* n_string_elements;
    IntegerPropertyItem* n_draw_steps;
    DoublePropertyItem* arrow_clamp_force;
    DoublePropertyItem* time_span_factor;
    DoublePropertyItem* time_step_factor;
    DoublePropertyItem* sampling_rate;
};
