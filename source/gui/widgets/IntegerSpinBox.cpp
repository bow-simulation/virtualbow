#include "IntegerSpinBox.hpp"
#include "gui/utils/IntegerRange.hpp"
#include <cmath>

calculate::Parser IntegerSpinBox::parser = calculate::Parser{};

IntegerSpinBox::IntegerSpinBox(const IntegerRange& range, QWidget* parent)
    : QSpinBox(parent)
{
    setMinimum(range.min);
    setMaximum(range.max);
}

int IntegerSpinBox::valueFromText(const QString& text) const {
    QString input = text;
    input.remove(suffix());

    auto expression = IntegerSpinBox::parser.parse(input.toStdString());
    return expression();
}

QValidator::State IntegerSpinBox::validate(QString& text, int& pos) const {
    QString input = text;
    input.remove(suffix());

    try {
        IntegerSpinBox::parser.parse(input.toStdString());
        return QValidator::Acceptable;
    }
    catch(calculate::BaseError&) {
        return QValidator::Intermediate;
    }
}

void IntegerSpinBox::stepBy(int steps) {
    QSpinBox::stepBy(steps);
    emit stepped();
}
