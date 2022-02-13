#include "StringPropertyItem.hpp"
#include "GroupPropertyItem.hpp"
#include <QLineEdit>

StringPropertyItem::StringPropertyItem(QString name, GroupPropertyItem* parent)
    : PropertyTreeItem(parent),
      name(name),
      value("Hello")
{
    this->setFlags(this->flags() | Qt::ItemIsEditable);
}

QVariant StringPropertyItem::data(int column, int role) const {
    if(column == 0 && role == Qt::DisplayRole) {
        return name;
    }

    if(column == 1 && (role == Qt::DisplayRole || role == Qt::EditRole)) {
        return value;
    }

    return PropertyTreeItem::data(column, role);
}

void StringPropertyItem::setData(int column, int role, const QVariant &value) {
    if(column == 1 && role == Qt::EditRole) {
        this->value = value.toString();
    }
}

QWidget* StringPropertyItem::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    QLineEdit* editor = new QLineEdit(parent);
    editor->setFrame(false);

    return editor;
}

void StringPropertyItem::setEditorData(QWidget* editor, const QModelIndex& index) const {
    QString value = index.model()->data(index, Qt::EditRole).toString();
    QLineEdit* lineEdit = static_cast<QLineEdit*>(editor);
    lineEdit->setText(value);
}

void StringPropertyItem::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const {
    QLineEdit* lineEdit = static_cast<QLineEdit*>(editor);
    QString value = lineEdit->text();

    model->setData(index, value, Qt::EditRole);
}
