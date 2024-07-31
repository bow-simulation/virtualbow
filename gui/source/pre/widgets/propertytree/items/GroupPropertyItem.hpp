#pragma once
#include "pre/widgets/propertytree/PropertyTreeItem.hpp"

class GroupPropertyItem: public PropertyTreeItem {
public:
    GroupPropertyItem(QString name, GroupPropertyItem* parent = nullptr);
};
