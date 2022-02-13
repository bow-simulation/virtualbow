#pragma once
#include "gui/utils/DoubleRange.hpp"
#include <QString>
#include <functional>

struct IntegerProperty_old {
    QString name;
    IntegerRange range;
    std::function<int()> get_value;
    std::function<void(int)> set_value;
};
