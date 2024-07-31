#include "PropertyTreeItem.hpp"

PropertyTreeItem::PropertyTreeItem(QTreeWidgetItem* parent)
    : QTreeWidgetItem(parent)
{

}

QVariant PropertyTreeItem::data(int column, int role) const {
    if(role == Qt::UserRole) {
        return QVariant::fromValue((intptr_t) this);
    }
    return QTreeWidgetItem::data(column, role);
}

QWidget* PropertyTreeItem::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    return nullptr;  // Default: no editor
}

void PropertyTreeItem::setEditorData(QWidget* editor, const QModelIndex& index) const {
    // Default: do nothing
}

void PropertyTreeItem::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const {
    // Default: do nothing
}
