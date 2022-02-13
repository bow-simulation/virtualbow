#pragma once
#include "gui/units/UnitGroup.hpp"
#include "gui/utils/DoubleRange.hpp"
#include <QString>
#include <functional>

struct DoubleProperty_old
{
    QString name;
    UnitGroup* unit;
    DoubleRange range;
    double stepsize;
    std::function<double()> get_value;
    std::function<void(double)> set_value;
};
