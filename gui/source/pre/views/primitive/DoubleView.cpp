#include "DoubleView.hpp"
#include "pre/utils/DoubleRange.hpp"
#include "pre/models/units/UnitSystem.hpp"
#include <QAbstractItemModel>
#include <QWheelEvent>
#include <cmath>

calculate::Parser DoubleView::parser = calculate::Parser{};

DoubleView::DoubleView(QAbstractItemModel* model, QPersistentModelIndex index, const Quantity& quantity, const DoubleRange& range):
    show_unit(true),
    quantity(quantity)
{
    setDecimals(DoubleRange::DECIMALS);
    setMinimum(range.min);
    setMaximum(range.max);
    setSingleStep(range.step);

    // Prevent catching focus when scrolling, https://stackoverflow.com/a/19382766
    setFocusPolicy(Qt::StrongFocus);

    // Keep value up to date
    QObject::connect(this, &QDoubleSpinBox::valueChanged, this, [=](double value){ model->setData(index, value); });
    setValue(model->data(index, Qt::DisplayRole).toDouble());

    // Keep unit up to date
    QObject::connect(&quantity, &Quantity::unitChanged, this, &DoubleView::updateUnit);
    updateUnit();
}

// Ignore scroll events if the widget doesn't have focus, https://stackoverflow.com/a/19382766
void DoubleView::wheelEvent(QWheelEvent *event) {
    if(!hasFocus()) {
        event->ignore();
    }
    else {
        QDoubleSpinBox::wheelEvent(event);
    }
}

void DoubleView::showUnit(bool value) {
    show_unit = value;
    updateUnit();
}

QString DoubleView::textFromValue(double value) const {
    double unit_value = quantity.getUnit().fromBase(value);
    if(std::abs(unit_value) < DoubleRange::EPSILON) {
        unit_value = 0.0;    // Round values that are close to zero to prevent them from showing as e.g. -9.93923e-17
    }

    return QString::number(unit_value, 'g', decimals());
}

double DoubleView::valueFromText(const QString& text) const {
    QString input = text;
    input.remove(suffix());

    auto expression = DoubleView::parser.parse(input.toStdString());
    double value = expression();

    return quantity.getUnit().toBase(value);
}

QValidator::State DoubleView::validate(QString& text, int& pos) const {
    QString input = text;
    input.remove(suffix());

    try {
        DoubleView::parser.parse(input.toStdString());
        return QValidator::Acceptable;
    }
    catch(calculate::BaseError&) {
        return QValidator::Intermediate;
    }
}

void DoubleView::stepBy(int steps) {
    QDoubleSpinBox::stepBy(steps);
    emit modified();
}

void DoubleView::updateUnit() {
    // Show the selected unit as suffix, if it is not "no unit"
    // Setting the suffix also triggers a new evaluation of textFromValue
    if(show_unit && quantity.getUnit() != Units::No_Unit) {
        setSuffix(" " + quantity.getUnit().getName());
    }
    else {
        setSuffix("");
    }
}
