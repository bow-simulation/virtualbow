#include "TableSpinner.hpp"
#include "gui/units/UnitGroup.hpp"
#include <limits>

TableSpinner::TableSpinner(const TableSpinnerOptions& options, QWidget* parent)
    : QDoubleSpinBox(parent)
{
    setSingleStep(options.units.getSelectedUnit().fromBase(options.step));    // Convert step size to selected unit
    setMinimum(options.range.min);
    setMaximum(options.range.max);
    setDecimals(8);  // Magic number
}

// Custom implementation without trailing zeros
QString TableSpinner::textFromValue(double value) const {
    return QString::number(value);    // Unit conversion is handled by the table model
}

double TableSpinner::valueFromText(const QString& text) const {
    return text.toDouble();    // Unit conversion is handled by the table model
}
