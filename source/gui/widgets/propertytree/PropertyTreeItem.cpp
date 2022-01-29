#include "PropertyTreeItem.hpp"

PropertyTreeItem::PropertyTreeItem(const QString& name, const QVariant& value, PropertyTreeItem* parent)
    : parent(parent),
      name(name),
      value(value)
{
    if(parent != nullptr) {
        parent->children.push_back(this);
    }
}

PropertyTreeItem::~PropertyTreeItem() {
    qDeleteAll(children);
}

const QString& PropertyTreeItem::getName() const {
    return name;
}

const QVariant& PropertyTreeItem::getValue() const {
    return value;
}

PropertyTreeItem *PropertyTreeItem::getParent() {
    return parent;
}

const QList<PropertyTreeItem*> PropertyTreeItem::getChildren() const {
    return children;
}
