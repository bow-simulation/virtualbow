#pragma once
#include "gui/treeview/TreeItem.hpp"

class DataViewModel;

class SettingsTreeItem: public TreeItem {
public:
    SettingsTreeItem(DataViewModel* model);
    QWidget* createEditor() const override;

private:
    DataViewModel* model;
};
