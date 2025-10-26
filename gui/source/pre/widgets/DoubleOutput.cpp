#include "DoubleOutput.hpp"

const QColor COLOR_OKAY = QColor("#99ff99");
const QColor COLOR_WARN = QColor("#ffdb99");
const QColor COLOR_FAIL = QColor("#ff9999");

DoubleOutput::DoubleOutput(double value, const Quantity& quantity, int decimals):
    value(value),
    decimals(decimals),
    quantity(quantity),
    allowedLimit(std::nullopt),
    maximumLimit(std::nullopt)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setAlignment(Qt::AlignCenter);
    setReadOnly(true);

    QObject::connect(&quantity, &Quantity::unitChanged, this, &DoubleOutput::updateAll);
    updateAll();
}

void DoubleOutput::setAllowedLimit(std::optional<double> value) {
    allowedLimit = value;
    updateLimits();
}

void DoubleOutput::setMaximumLimit(std::optional<double> value) {
    maximumLimit = value;
    updateLimits();
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
        setToolTip("Value exceeds the maximum threshold of " + maximum);
        palette.setColor(QPalette::Base, COLOR_FAIL);
    }
    else {
        QString allowed = QString::number(quantity.getUnit().fromBase(*allowedLimit), 'f', decimals) + quantity.getUnit().getSuffix();
        if(value > *allowedLimit) {
            setToolTip("Value exceeds the allowed threshold of " + allowed);
            palette.setColor(QPalette::Base, COLOR_WARN);
        }
        else {
            setToolTip("Value is within the allowed threshold of " + allowed);
            palette.setColor(QPalette::Base, COLOR_OKAY);
        }
    }

    setPalette(palette);
}
