#include "DoublePropertyItem.hpp"
#include "GroupPropertyItem.hpp"
#include <QDoubleSpinBox>

DoublePropertyItem::DoublePropertyItem(const DoubleProperty& property, GroupPropertyItem* parent)
    : PropertyTreeItem(parent),
      property(property)
{
    this->setFlags(this->flags() | Qt::ItemIsEditable);
    QObject::connect(this->property.unit, &UnitGroup::selectionChanged, [&]{ emitDataChanged(); });    // Update on unit changes
}

QVariant DoublePropertyItem::data(int column, int role) const {
    if(column == 0 && role == Qt::DisplayRole) {
        return property.name;
    }

    // For display role, convert the value to the specified unit and return as a string with unit label.
    // For the edit role, convert the value and return as double to be used by the editor
    if(column == 1) {
        double unit_value = property.unit->getSelectedUnit().fromBase(property.get_value());
        if(role == Qt::DisplayRole) {
            return QString::number(unit_value) + " " + property.unit->getSelectedUnit().getName();
        }
        if(role == Qt::EditRole) {
            return unit_value;
        }
    }

    return PropertyTreeItem::data(column, role);
}

void DoublePropertyItem::setData(int column, int role, const QVariant &value) {
    if(column == 1 && role == Qt::EditRole) {
        double base_value = property.unit->getSelectedUnit().toBase(value.toDouble());
        property.set_value(base_value);
    }
}

QWidget* DoublePropertyItem::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    // Assumes that the unit doesn't change during the lifetime of the editor
    QDoubleSpinBox* editor = new QDoubleSpinBox(parent);
    editor->setFrame(false);
    editor->setMinimum(property.unit->getSelectedUnit().fromBase(property.range.min));
    editor->setMaximum(property.unit->getSelectedUnit().fromBase(property.range.max));
    editor->setSingleStep(property.unit->getSelectedUnit().fromBase(property.stepsize));
    editor->setSuffix(" " + property.unit->getSelectedUnit().getName());

    return editor;
}

void DoublePropertyItem::setEditorData(QWidget* editor, const QModelIndex& index) const {
    double value = index.model()->data(index, Qt::EditRole).toDouble();
    auto spinBox = static_cast<QDoubleSpinBox*>(editor);
    spinBox->setValue(value);
}

void DoublePropertyItem::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const {
    QDoubleSpinBox* spinBox = static_cast<QDoubleSpinBox*>(editor);
    spinBox->interpretText();
    double value = spinBox->value();

    model->setData(index, value, Qt::EditRole);
}
