#pragma once
#include "pre/models/units/Quantity.hpp"
#include <QLineEdit>

class DoubleOutput: public QLineEdit {
public:
    DoubleOutput(double value, const Quantity& quantity, int decimals, const QString& tooltip);
    DoubleOutput(double value, double allowed, double maximum, const Quantity& quantity, int decimals, const QString& tooltip);

private:
    const Quantity& quantity;
    double value;
    int decimals;
    QString tooltip;

    std::optional<double> allowedLimit;
    std::optional<double> maximumLimit;

    void updateAll();
    void updateLimits();
};
