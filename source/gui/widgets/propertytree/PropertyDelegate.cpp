#include "PropertyDelegate.hpp"
#include "PropertyTreeItem.hpp"

#include <QDoubleSpinBox>
#include <QDebug>
#include <QTreeWidgetItem>

PropertyDelegate2::PropertyDelegate2(QObject* parent)
    : QStyledItemDelegate(parent)
{

}

QWidget* PropertyDelegate2::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    if(index.column() == 1) {
        auto data = index.data(Qt::UserRole);
        auto item = (PropertyTreeItem*) data.value<intptr_t>();

        if(item != nullptr) {
            return item->createEditor(parent, option, index);
        }
    }

    return nullptr;    // Prevent editing the first column (https://stackoverflow.com/a/4964668)
}

void PropertyDelegate2::setEditorData(QWidget* editor, const QModelIndex& index) const {
    auto data = index.data(Qt::UserRole);
    auto item = (PropertyTreeItem*) data.value<intptr_t>();

    if(item != nullptr) {
        item->setEditorData(editor, index);
    }
}

void PropertyDelegate2::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const {
    auto data = index.data(Qt::UserRole);
    auto item = (PropertyTreeItem*) data.value<intptr_t>();

    if(item != nullptr) {
        item->setModelData(editor, model, index);
    }
}

void PropertyDelegate2::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    editor->setGeometry(option.rect);
}
