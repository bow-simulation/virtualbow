#pragma once
#include <QString>
#include <functional>

struct StringProperty_old {
    QString name;
    std::function<QString()> get_value;
    std::function<void(const QString&)> set_value;
};
