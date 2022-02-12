#pragma once
#include "gui/treeview/TreeItem.hpp"

class DataViewModel;

class CommentTreeItem: public TreeItem {
public:
    CommentTreeItem(DataViewModel* model);
    QWidget* createEditor() const override;

private:
    DataViewModel* model;
};
