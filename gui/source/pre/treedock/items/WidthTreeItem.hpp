#pragma once
#include "pre/treedock/TreeItem.hpp"
#include "solver/Input.hpp"

class ViewModel;

class WidthTreeItem: public TreeItem {
public:
    WidthTreeItem(ViewModel* model);
    void updateModel(void* source);
    void updateView(void *source);
};
