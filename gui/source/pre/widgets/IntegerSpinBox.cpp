#include "IntegerSpinBox.hpp"
#include "pre/utils/Expressions.hpp"
#include "pre/utils/IntegerRange.hpp"
#include <cmath>

IntegerSpinBox::IntegerSpinBox(const IntegerRange& range, QWidget* parent)
    : QSpinBox(parent)
{
    setMinimum(range.min);
    setMaximum(range.max);

    // Prevent catching focus when scrolling, https://stackoverflow.com/a/19382766
    setFocusPolicy(Qt::StrongFocus);
}

int IntegerSpinBox::valueFromText(const QString& text) const {
    QString input = text;
    input.remove(suffix());

    auto expression = parseExpression(input);
    return expression();
}

QValidator::State IntegerSpinBox::validate(QString& text, int& pos) const {
    QString input = text;
    input.remove(suffix());

    if(!checkExpression(input)) {
        return QValidator::Intermediate;
    }

    return QValidator::Acceptable;
}
