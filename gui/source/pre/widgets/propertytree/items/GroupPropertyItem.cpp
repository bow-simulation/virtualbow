#include "GroupPropertyItem.hpp"

GroupPropertyItem::GroupPropertyItem(QString name, GroupPropertyItem* parent)
    : PropertyTreeItem(parent)
{
    QFont font;
    font.setBold(true);

    this->setFont(0, font);
    this->setText(0, name);
}
