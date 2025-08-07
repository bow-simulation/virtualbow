#include "DoubleSpinBox.hpp"
#include "pre/utils/DoubleRange.hpp"
#include "pre/utils/Expressions.hpp"
#include "pre/models/units/UnitSystem.hpp"
#include <cmath>

DoubleSpinBox::DoubleSpinBox(const Quantity& quantity, const DoubleRange& range, QWidget* parent):
    QDoubleSpinBox(parent),
    range(range),
    show_unit(true),
    quantity(quantity)
{
    setDecimals(8);    // Magic number
    setSingleStep(range.step);
    setMinimum(-std::numeric_limits<double>::infinity());    // Not used
    setMaximum(std::numeric_limits<double>::infinity());     // Not used

    // Prevent catching focus when scrolling, https://stackoverflow.com/a/19382766
    setFocusPolicy(Qt::StrongFocus);

    QObject::connect(&quantity, &Quantity::unitChanged, this, &DoubleSpinBox::updateUnit);
    updateUnit();
}

void DoubleSpinBox::showUnit(bool value) {
    show_unit = value;
    updateUnit();
}

QString DoubleSpinBox::textFromValue(double baseValue) const {
    // Convert value given in SI base to the selected unit
    double unitValue = quantity.getUnit().fromBase(baseValue);

    // Convert value to string with fixed-point representation
    // If the result has a decimal point, remove any trailing zeros and the point as well
    QString result = QString::number(unitValue, 'f', decimals());
    if(result.indexOf('.') != -1) {
        while(result.endsWith('0')) {
            result.chop(1);
        }
        if(result.endsWith('.')) {
            result.chop(1);
        }
    }

    return result;
}

double DoubleSpinBox::valueFromText(const QString& text) const {
    // Remove the (unit) suffix from the text
    QString input = text;
    input.remove(suffix());

    // Convert value given in the selected unit to SI base
    double unitValue = evalExpression(input);
    return quantity.getUnit().toBase(unitValue);
}

// Input must be a valid arithmetic expression and be contained in the valid range.
// Only ever returns QValidator::Intermediate on inputs that don't fulfill this because they could be fixed by typing more characters.
QValidator::State DoubleSpinBox::validate(QString& text, int& pos) const {
    QString input = text;
    input.remove(suffix());

    try {
        double value = evalExpression(input);
        if(range.contains(value)) {
            return QValidator::Acceptable;
        }
        else {
            return QValidator::Intermediate;
        }
    }
    catch(const std::runtime_error&) {
        return QValidator::Intermediate;
    }
}

// Overwriting this ensures that the range validation also applies to steps made by "spinning"
void DoubleSpinBox::stepBy(int steps) {
    double newVal = value() + singleStep() * steps;
    if(range.contains(newVal)) {
        setValue(newVal);
    }
}

void DoubleSpinBox::updateUnit() {
    // Show the selected unit as suffix, if it is not "no unit"
    // Setting the suffix also triggers a new evaluation of textFromValue
    if(show_unit && quantity.getUnit() != Units::No_Unit) {
        setSuffix(" " + quantity.getUnit().getLabel());
    }
    else {
        setSuffix("");
    }
}
