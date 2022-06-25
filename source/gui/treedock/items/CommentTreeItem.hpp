#pragma once
#include "gui/treedock/TreeItem.hpp"
#include <string>

class ViewModel;
class QPlainTextEdit;

class CommentTreeItem: public TreeItem
{
public:
    CommentTreeItem(ViewModel* model);
    void updateModel(void* source) override;
    void updateView(void* source) override;
};
