#include "IntegerTreeItem.hpp"
#include "GroupTreeItem.hpp"
#include <QSpinBox>

IntegerTreeItem2::IntegerTreeItem2(QString name, GroupTreeItem* parent)
    : PropertyTreeItem(parent),
      name(name),
      value(25)
{
    this->setFlags(this->flags() | Qt::ItemIsEditable);
}

QVariant IntegerTreeItem2::data(int column, int role) const {
    if(column == 0 && role == Qt::DisplayRole) {
        return name;
    }

    if(column == 1 && (role == Qt::DisplayRole || role == Qt::EditRole)) {
        return value;
    }

    return PropertyTreeItem::data(column, role);
}

void IntegerTreeItem2::setData(int column, int role, const QVariant &value) {
    if(column == 1 && role == Qt::EditRole) {
        this->value = value.toInt();
    }
}

QWidget* IntegerTreeItem2::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    QSpinBox* editor = new QSpinBox(parent);
    editor->setFrame(false);
    editor->setMinimum(0);
    editor->setMaximum(50);

    return editor;
}

void IntegerTreeItem2::setEditorData(QWidget* editor, const QModelIndex& index) const {
    int value = index.model()->data(index, Qt::EditRole).toInt();
    QSpinBox* spinBox = static_cast<QSpinBox*>(editor);
    spinBox->setValue(value);
}

void IntegerTreeItem2::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const {
    QSpinBox* spinBox = static_cast<QSpinBox*>(editor);
    spinBox->interpretText();    // From Qt examples, to make sure the latest changes are included
    int value = spinBox->value();

    model->setData(index, value, Qt::EditRole);
}
