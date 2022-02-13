#pragma once
#include "gui/widgets/propertytree/PropertyTreeItem.hpp"

class GroupPropertyItem: public PropertyTreeItem {
public:
    GroupPropertyItem(QString name, GroupPropertyItem* parent = nullptr);
};
