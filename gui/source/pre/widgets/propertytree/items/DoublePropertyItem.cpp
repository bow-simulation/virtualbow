#include "DoublePropertyItem.hpp"
#include "GroupPropertyItem.hpp"
#include "pre/widgets/DoubleSpinBox.hpp"

DoublePropertyItem::DoublePropertyItem(const QString& name, const Quantity& quantity, const DoubleRange& range, GroupPropertyItem* parent)
    : PropertyTreeItem(parent),
      name(name),
      quantity(quantity),
      range(range),
      value(0.0)

{
    this->setFlags(this->flags() | Qt::ItemIsEditable);
    QObject::connect(&quantity, &Quantity::unitChanged, [&]{ emitDataChanged(); });    // Update on unit changes
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
    // For the edit role, return the base value because the editor widget deals with the units.
    if(column == 1) {
        double unit_value = quantity.getUnit().fromBase(value);
        if(role == Qt::DisplayRole) {
            return QString::number(unit_value) + " " + quantity.getUnit().getName();
        }
        if(role == Qt::EditRole) {
            return value;
        }
    }

    return PropertyTreeItem::data(column, role);
}

void DoublePropertyItem::setData(int column, int role, const QVariant &value) {
    if(column == 1 && role == Qt::EditRole) {
        this->value = value.toDouble();
        emitDataChanged();
    }
}

QWidget* DoublePropertyItem::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    auto editor = new DoubleSpinBox(quantity, range, parent);
    editor->setFrame(false);

    return editor;
}

void DoublePropertyItem::setEditorData(QWidget* editor, const QModelIndex& index) const {
    double value = index.model()->data(index, Qt::EditRole).toDouble();
    auto spinner = static_cast<DoubleSpinBox*>(editor);
    spinner->setValue(value);
}

void DoublePropertyItem::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const {
    auto spinner = static_cast<DoubleSpinBox*>(editor);
    spinner->interpretText();
    double value = spinner->value();

    model->setData(index, value, Qt::EditRole);
}
