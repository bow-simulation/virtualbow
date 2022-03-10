#pragma once
#include "gui/treedock/TreeItem.hpp"
#include <string>

class DataViewModel;
class QPlainTextEdit;

class CommentTreeItem: public TreeItem
{
public:
    CommentTreeItem(DataViewModel* model);
    void updateModel();
    void updateView();

private:
    DataViewModel* model;
};
