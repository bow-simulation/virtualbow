#include "GroupTreeItem.hpp"

GroupTreeItem::GroupTreeItem(QString name, GroupTreeItem* parent)
    : PropertyTreeItem(parent)
{
    QFont font;
    font.setBold(true);

    this->setFont(0, font);
    this->setText(0, name);
}
