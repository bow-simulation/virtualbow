#include "DoubleSpinBox.hpp"
#include "gui/utils/DoubleRange.hpp"
#include "gui/viewmodel/units/UnitGroup.hpp"
#include <limits>
#include <cmath>

calculate::Parser DoubleSpinBox::parser = calculate::Parser{};

DoubleSpinBox::DoubleSpinBox(const UnitGroup& units, const DoubleRange& range, QWidget* parent)
    : QDoubleSpinBox(parent),
      units(units)
{
    setDecimals(std::numeric_limits<int>::max());
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
    emit stepped();
}

void DoubleSpinBox::updateUnit() {
    setSuffix(" " + units.getSelectedUnit().getName());    // This also triggers a new evaluation of textFromValue
}
