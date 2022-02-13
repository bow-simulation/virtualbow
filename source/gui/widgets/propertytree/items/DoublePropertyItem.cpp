#include "DoublePropertyItem.hpp"
#include "GroupPropertyItem.hpp"
#include <QDoubleSpinBox>

DoublePropertyItem::DoublePropertyItem(const QString& name, const UnitGroup* units, const DoubleRange& range, double step, GroupPropertyItem* parent)
    : PropertyTreeItem(parent),
      name(name),
      units(units),
      range(range),
      step(step),
      value(0.0)

{
    this->setFlags(this->flags() | Qt::ItemIsEditable);
    QObject::connect(units, &UnitGroup::selectionChanged, [&]{ emitDataChanged(); });    // Update on unit changes
}

double DoublePropertyItem::getValue() const {
    return value;
}

void DoublePropertyItem::setValue(double value) {
    this->value = value;
    emitDataChanged();
}

QVariant DoublePropertyItem::data(int column, int role) const {
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

void DoublePropertyItem::setData(int column, int role, const QVariant &value) {
    if(column == 1 && role == Qt::EditRole) {
        double base_value = units->getSelectedUnit().toBase(value.toDouble());
        this->value = base_value;
        emitDataChanged();
    }
}

QWidget* DoublePropertyItem::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    // Assumes that the unit doesn't change during the lifetime of the editor
    QDoubleSpinBox* editor = new QDoubleSpinBox(parent);
    editor->setFrame(false);
    editor->setMinimum(units->getSelectedUnit().fromBase(range.min));
    editor->setMaximum(units->getSelectedUnit().fromBase(range.max));
    editor->setSingleStep(units->getSelectedUnit().fromBase(step));
    editor->setSuffix(" " + units->getSelectedUnit().getName());

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
