#include "IntegerPropertyItem.hpp"
#include "GroupPropertyItem.hpp"
#include <QSpinBox>

IntegerPropertyItem::IntegerPropertyItem(const QString& name, const IntegerRange& range, GroupPropertyItem* parent)
    : PropertyTreeItem(parent),
      name(name),
      range(range),
      value(0)
{
    this->setFlags(this->flags() | Qt::ItemIsEditable);
}

int IntegerPropertyItem::getValue() const {
    return value;
}

void IntegerPropertyItem::setValue(int value) {
    this->value = value;
    emitDataChanged();
}

QVariant IntegerPropertyItem::data(int column, int role) const {
    if(column == 0 && role == Qt::DisplayRole) {
        return name;
    }

    if(column == 1 && (role == Qt::DisplayRole || role == Qt::EditRole)) {
        return value;
    }

    return PropertyTreeItem::data(column, role);
}

void IntegerPropertyItem::setData(int column, int role, const QVariant &value) {
    if(column == 1 && role == Qt::EditRole) {
        this->value = value.toInt();
        emitDataChanged();
    }
}

QWidget* IntegerPropertyItem::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    QSpinBox* editor = new QSpinBox(parent);
    editor->setFrame(false);
    editor->setMinimum(range.min);
    editor->setMaximum(range.max);

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
