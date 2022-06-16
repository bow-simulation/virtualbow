#include "DoubleSpinBox.hpp"
#include "gui/utils/DoubleRange.hpp"
#include "gui/viewmodel/units/UnitGroup.hpp"

DoubleSpinBox::DoubleSpinBox(const UnitGroup& units, const DoubleRange& range, QWidget* parent)
    : QDoubleSpinBox(parent),
      units(units)
{
    setDecimals(6);        // Magic number
    setMinimum(range.min);
    setMaximum(range.max);
    setSingleStep(range.step);

    QObject::connect(&units, &UnitGroup::selectionChanged, this, &DoubleSpinBox::updateUnit);
    updateUnit();
}

QString DoubleSpinBox::textFromValue(double value) const {
    return QString::number(units.getSelectedUnit().fromBase(value));
}

double DoubleSpinBox::valueFromText(const QString& text) const {
    QString number = text;
    number.remove(suffix());
    double value = number.toDouble();
    return units.getSelectedUnit().toBase(value);
}

void DoubleSpinBox::updateUnit() {
    setSuffix(" " + units.getSelectedUnit().getName());    // This also triggers a new evaluation of textFromValue
}
