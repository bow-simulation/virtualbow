#include "DoubleSpinBox.hpp"
#include "gui/utils/DoubleRange.hpp"
#include "gui/viewmodel/units/UnitGroup.hpp"
#include "gui/viewmodel/units/Definitions.hpp"
#include <limits>
#include <cmath>

calculate::Parser DoubleSpinBox::parser = calculate::Parser{};

DoubleSpinBox::DoubleSpinBox(const UnitGroup& units, const DoubleRange& range, QWidget* parent)
    : QDoubleSpinBox(parent),
      show_unit(true),
      units(units)
{
    qInfo() << "Tolerance:" << DoubleRange::EPSILON;

    setDecimals(DoubleRange::DECIMALS);
    setMinimum(range.min);
    setMaximum(range.max);
    setSingleStep(range.step);

    QObject::connect(this, &DoubleSpinBox::editingFinished, this, &DoubleSpinBox::modified);
    QObject::connect(&units, &UnitGroup::selectionChanged, this, &DoubleSpinBox::updateUnit);
    updateUnit();
}

void DoubleSpinBox::showUnit(bool value) {
    show_unit = value;
    updateUnit();
}

QString DoubleSpinBox::textFromValue(double value) const {
    double unit_value = units.getSelectedUnit().fromBase(value);
    if(std::abs(unit_value) < DoubleRange::EPSILON) {
        unit_value = 0.0;    // Round values that are close to zero to prevent them from showing as e.g. -9.93923e-17
    }

    return QString::number(unit_value, 'g', decimals());
}

double DoubleSpinBox::valueFromText(const QString& text) const {
    QString input = text;
    input.remove(suffix());

    auto expression = DoubleSpinBox::parser.parse(input.toStdString());
    double value = expression();

    return units.getSelectedUnit().toBase(value);
}

QValidator::State DoubleSpinBox::validate(QString& text, int& pos) const {
    QString input = text;
    input.remove(suffix());

    try {
        DoubleSpinBox::parser.parse(input.toStdString());
        return QValidator::Acceptable;
    }
    catch(calculate::BaseError&) {
        return QValidator::Intermediate;
    }
}

void DoubleSpinBox::stepBy(int steps) {
    QDoubleSpinBox::stepBy(steps);
    emit modified();
}

void DoubleSpinBox::updateUnit() {
    // Show the selected unit as suffix, if it is not "no unit"
    // Setting the suffix also triggers a new evaluation of textFromValue
    if(show_unit && units.getSelectedUnit() != Units::No_Unit) {
        setSuffix(" " + units.getSelectedUnit().getName());
    }
    else {
        setSuffix("");
    }
}
