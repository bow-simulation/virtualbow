#include "DoubleSpinBox.hpp"
#include "gui/viewmodel/units/UnitGroup.hpp"
#include <limits>

DoubleSpinBox::DoubleSpinBox(Domain range, QWidget* parent)
    : QDoubleSpinBox(parent),
      units(nullptr)
{
    setDecimals(6);        // Magic number
    setSingleStep(0.1);    // Magic number

    switch(range) {
    case Domain::UNRESTRICTED:
        setMinimum(std::numeric_limits<double>::lowest());
        setMaximum(std::numeric_limits<double>::max());
        break;

    case Domain::POSITIVE:
        setMinimum(std::numeric_limits<double>::min());
        setMaximum(std::numeric_limits<double>::max());
        break;

    case Domain::NEGATIVE:
        setMinimum(std::numeric_limits<double>::lowest());
        setMaximum(-std::numeric_limits<double>::min());
        break;

    case Domain::NON_POSITIVE:
        setMinimum(std::numeric_limits<double>::lowest());
        setMaximum(0.0);
        break;

    case Domain::NON_NEGATIVE:
        setMinimum(0.0);
        setMaximum(std::numeric_limits<double>::max());
        break;
    }
}

void DoubleSpinBox::setUnitGroup(const UnitGroup* new_units) {
    if(units != nullptr) {
        QObject::disconnect(units, &UnitGroup::selectionChanged, this, &DoubleSpinBox::updateUnit);
    }
    if(new_units != nullptr) {
        QObject::connect(new_units, &UnitGroup::selectionChanged, this, &DoubleSpinBox::updateUnit);
    }
    units = new_units;
    updateUnit();
}

QString DoubleSpinBox::textFromValue(double value) const {
    if(units != nullptr) {
        return QString::number(units->getSelectedUnit().fromBase(value));
    }
    else {
        return QString::number(value);
    }
}

double DoubleSpinBox::valueFromText(const QString& text) const {
    QString number = text;
    number.remove(suffix());
    double value = QLocale().toDouble(number);

    if(units != nullptr) {
        return units->getSelectedUnit().toBase(value);
    }
    else {
        return value;
    }
}

void DoubleSpinBox::updateUnit() {
    setSuffix(" " + units->getSelectedUnit().getName());    // This also triggers a new evaluation of textFromValue
    setSingleStep(units->getSelectedUnit().toBase(0.1));    // Magic number
}
