#include "IntegerPropertyItem.hpp"
#include "GroupPropertyItem.hpp"
#include <QSpinBox>

IntegerPropertyItem::IntegerPropertyItem(const IntegerProperty& property, GroupPropertyItem* parent)
    : PropertyTreeItem(parent),
      property(property)
{
    this->setFlags(this->flags() | Qt::ItemIsEditable);
}

QVariant IntegerPropertyItem::data(int column, int role) const {
    if(column == 0 && role == Qt::DisplayRole) {
        return property.name;
    }

    if(column == 1 && (role == Qt::DisplayRole || role == Qt::EditRole)) {
        return property.get_value();
    }

    return PropertyTreeItem::data(column, role);
}

void IntegerPropertyItem::setData(int column, int role, const QVariant &value) {
    if(column == 1 && role == Qt::EditRole) {
        property.set_value(value.toInt());
    }
}

QWidget* IntegerPropertyItem::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    QSpinBox* editor = new QSpinBox(parent);
    editor->setFrame(false);
    editor->setMinimum(property.range.min);
    editor->setMaximum(property.range.max);

    return editor;
}

void IntegerPropertyItem::setEditorData(QWidget* editor, const QModelIndex& index) const {
    int value = index.model()->data(index, Qt::EditRole).toInt();
    QSpinBox* spinBox = static_cast<QSpinBox*>(editor);
    spinBox->setValue(value);
}

void IntegerPropertyItem::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const {
    QSpinBox* spinBox = static_cast<QSpinBox*>(editor);
    spinBox->interpretText();    // From Qt examples, to make sure the latest changes are included
    int value = spinBox->value();

    model->setData(index, value, Qt::EditRole);
}
