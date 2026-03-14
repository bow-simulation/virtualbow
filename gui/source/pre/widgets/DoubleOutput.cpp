#include "DoubleOutput.hpp"

const QColor COLOR_OKAY = QColor("#99ff99");
const QColor COLOR_WARN = QColor("#ffdb99");
const QColor COLOR_FAIL = QColor("#ff9999");

DoubleOutput::DoubleOutput(double value, const Quantity& quantity, int decimals, const QString& tooltip):
    value(value),
    decimals(decimals),
    quantity(quantity),
    tooltip(tooltip),
    allowedLimit(std::nullopt),
    maximumLimit(std::nullopt)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setAlignment(Qt::AlignCenter);
    setReadOnly(true);
    setToolTip(tooltip);

    QObject::connect(&quantity, &Quantity::unitChanged, this, &DoubleOutput::updateAll);
    updateAll();
}

DoubleOutput::DoubleOutput(double value, double allowed, double maximum, const Quantity& quantity, int decimals, const QString& tooltip):
    DoubleOutput(value, quantity, decimals, tooltip)
{
    allowedLimit = allowed;
    maximumLimit = maximum;
    updateAll();
}

void DoubleOutput::updateAll() {
    QString text = QString::number(quantity.getUnit().fromBase(value), 'f', decimals) + quantity.getUnit().getSuffix();
    setText(text);

    updateLimits();
}

void DoubleOutput::updateLimits() {
    if(!allowedLimit.has_value() || !maximumLimit.has_value()) {
        return;
    }

    QPalette palette;
    if(value > *maximumLimit) {
        QString maximum = QString::number(quantity.getUnit().fromBase(*maximumLimit), 'f', decimals) + quantity.getUnit().getSuffix();
        setToolTip(tooltip + "\nValue exceeds the maximum threshold of " + maximum);
        palette.setColor(QPalette::Base, COLOR_FAIL);
    }
    else {
        QString allowed = QString::number(quantity.getUnit().fromBase(*allowedLimit), 'f', decimals) + quantity.getUnit().getSuffix();
        if(value > *allowedLimit) {
            setToolTip(tooltip + "\nValue exceeds the allowed threshold of " + allowed);
            palette.setColor(QPalette::Base, COLOR_WARN);
        }
        else {
            setToolTip(tooltip + "\nValue is within the allowed threshold of " + allowed);
            palette.setColor(QPalette::Base, COLOR_OKAY);
        }
    }

    setPalette(palette);
}
