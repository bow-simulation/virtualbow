#include "DoubleSpinBox.hpp"
#include "pre/utils/DoubleRange.hpp"
#include "pre/utils/Expressions.hpp"
#include "pre/utils/Rounding.hpp"
#include "pre/models/units/UnitSystem.hpp"
#include <QLineEdit>
#include <cmath>

DoubleSpinBox::DoubleSpinBox(const Quantity& quantity, const DoubleRange& range, QWidget* parent):
    QDoubleSpinBox(parent),
    range(range),
    show_unit(true),
    quantity(quantity)
{
    setDecimals(16);    // High precision since this only applies to the internal value stored in base unit. Conversion and rounding is done in textFromValue.
    setMinimum(-std::numeric_limits<double>::infinity());    // Not used
    setMaximum(std::numeric_limits<double>::infinity());     // Not used

    // Prevent catching focus when scrolling, https://stackoverflow.com/a/19382766
    setFocusPolicy(Qt::StrongFocus);

    // Signal modification by user when the text was edited
    // QDoubleSpinBox::valueChanged is not used because it is triggered during typing, leading to unnecessary updates of the model.
    QObject::connect(this, &QDoubleSpinBox::editingFinished, this, &DoubleSpinBox::contentModified);
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

    // Convert value to string with fixed-point representation and limited precision for display
    // If the result has a decimal point, remove any trailing zeros and possibly the point as well
    QString result = QString::number(unitValue, 'f', 6);
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
    double unitValue = *evalExpression(input);
    return quantity.getUnit().toBase(unitValue);
}

// Input must be a valid arithmetic expression and be contained in the valid range.
// Only ever returns QValidator::Intermediate on inputs that don't fulfill this because they could be fixed by typing more characters.
QValidator::State DoubleSpinBox::validate(QString& text, int& pos) const {
    QString input = text;
    input.remove(suffix());

    std::optional<double> unitValue = evalExpression(input);
    if(!unitValue.has_value()) {
        return QValidator::Intermediate;
    }

    double baseValue = quantity.getUnit().toBase(*unitValue);
    if(range.contains(baseValue)) {
        return QValidator::Acceptable;
    }
    else {
        return QValidator::Intermediate;
    }
}

// Overwriting this ensures that the range validation also applies to steps made by "spinning"
void DoubleSpinBox::stepBy(int steps) {
    double newValue = value() + singleStep()*steps;    // Compute new value by applying step
    if(range.contains(newValue)) {
        setValue(newValue);
        emit contentModified();    // Signal modification by user
    }
}

void DoubleSpinBox::updateUnit() {
    // Show the selected unit as suffix
    // Setting the suffix also triggers a new evaluation of textFromValue
    setSuffix(quantity.getUnit().getSuffix());

    // Select the step so that is has a smooth value when converted to the unit
    double unitStep = quantity.getUnit().fromBase(range.step);    // Convert the preferred step size to the unít
    unitStep = floorToPow10(unitStep);                            // Make the unit step a smooth value by rounding down to the nearest power of 10

    setSingleStep(quantity.getUnit().toBase(unitStep));     // Convert back to base value and apply
}
