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

    // Signal modification by user when the text was edited
    // QSpinBox::valueChanged is not used because it is triggered during typing, leading to unnecessary updates of the model.
    QObject::connect(this, &QSpinBox::editingFinished, this, &IntegerSpinBox::contentModified);
}

int IntegerSpinBox::valueFromText(const QString& text) const {
    QString input = text;
    input.remove(suffix());

    return *evalExpression(input);
}

QValidator::State IntegerSpinBox::validate(QString& text, int& pos) const {
    QString input = text;
    input.remove(suffix());

    if(!checkExpression(input)) {
        return QValidator::Intermediate;
    }

    return QValidator::Acceptable;
}

void IntegerSpinBox::stepBy(int steps) {
    QSpinBox::stepBy(steps);
    emit contentModified();    // Signal modification by user when the widget was stepped
}
