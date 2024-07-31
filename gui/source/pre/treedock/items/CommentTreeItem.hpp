#pragma once
#include "pre/treedock/TreeItem.hpp"
#include <string>

class ViewModel;
class QPlainTextEdit;

class CommentTreeItem: public TreeItem
{
public:
    CommentTreeItem(ViewModel* model);
    void updateModel(void* source);
    void updateView(void* source);
};
