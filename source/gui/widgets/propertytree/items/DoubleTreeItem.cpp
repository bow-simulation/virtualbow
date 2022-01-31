#include "DoubleTreeItem.hpp"
#include "GroupTreeItem.hpp"
#include <QDoubleSpinBox>

DoubleTreeItem::DoubleTreeItem(QString name, GroupTreeItem* parent)
    : PropertyTreeItem(parent),
      name(name),
      value(1.5),
      units(&UnitSystem::length)
{
    this->setFlags(this->flags() | Qt::ItemIsEditable);
    QObject::connect(units, &UnitGroup::selectionChanged, [&]{ emitDataChanged(); });    // Update on unit changes
}

QVariant DoubleTreeItem::data(int column, int role) const {
    if(column == 0 && role == Qt::DisplayRole) {
        return name;
    }

    // For display role, convert the value to the specified unit and return as a string with unit label.
    // For the edit role, convert the value and return as double to be used by the editor
    if(column == 1) {
        double unit_value = units->getSelectedUnit().fromBase(value);
        if(role == Qt::DisplayRole) {
            return QString::number(unit_value) + " " + units->getSelectedUnit().getName();
        }
        if(role == Qt::EditRole) {
            return unit_value;
        }
    }

    return PropertyTreeItem::data(column, role);
}

void DoubleTreeItem::setData(int column, int role, const QVariant &value) {
    if(column == 1 && role == Qt::EditRole) {
        this->value = units->getSelectedUnit().toBase(value.toDouble());
    }
}

QWidget* DoubleTreeItem::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    // Assumes that the unit doesn't change during the lifetime of the editor
    QDoubleSpinBox* editor = new QDoubleSpinBox(parent);
    editor->setFrame(false);
    editor->setMinimum(0.0);
    editor->setMaximum(100.0);
    editor->setSuffix(" " + units->getSelectedUnit().getName());

    return editor;
}

void DoubleTreeItem::setEditorData(QWidget* editor, const QModelIndex& index) const {
    int value = index.model()->data(index, Qt::EditRole).toDouble();
    QDoubleSpinBox* spinBox = static_cast<QDoubleSpinBox*>(editor);
    spinBox->setValue(value);
}

void DoubleTreeItem::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const {
    QDoubleSpinBox* spinBox = static_cast<QDoubleSpinBox*>(editor);
    spinBox->interpretText();
    double value = spinBox->value();

    model->setData(index, value, Qt::EditRole);
}
