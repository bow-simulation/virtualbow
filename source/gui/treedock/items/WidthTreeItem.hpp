#pragma once
#include "gui/treedock/TreeItem.hpp"
#include "solver/model/input/InputData.hpp"

class ViewModel;

class WidthTreeItem: public TreeItem {
public:
    WidthTreeItem(ViewModel* model);
    void updateModel(void* source) override;
    void updateView(void *source) override;
};
