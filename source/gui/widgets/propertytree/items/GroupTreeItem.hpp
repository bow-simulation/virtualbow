#pragma once
#include "gui/widgets/propertytree/PropertyTreeItem.hpp"

class GroupTreeItem: public PropertyTreeItem {
public:
    GroupTreeItem(QString name, GroupTreeItem* parent = nullptr);
};
