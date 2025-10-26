#pragma once
#include "pre/models/units/Quantity.hpp"
#include <QLineEdit>

class DoubleOutput: public QLineEdit {
public:
    DoubleOutput(double value, const Quantity& quantity, int decimals = 2);

    void setAllowedLimit(std::optional<double> value);
    void setMaximumLimit(std::optional<double> value);

private:
    const Quantity& quantity;
    double value;
    int decimals;

    std::optional<double> allowedLimit;
    std::optional<double> maximumLimit;

    void updateAll();
    void updateLimits();
};
