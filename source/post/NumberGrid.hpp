#pragma once
#include "gui/units/UnitGroup.hpp"
#include <QtWidgets>

class NumberGrid : public QWidget {
public:
    NumberGrid();

    void addColumn();
    void addGroup(const QString& name);
    void addValue(const QString& name, double value, const UnitGroup& unit);

private:
    QHBoxLayout* columnLayout;
    QVBoxLayout* currentColumn;
    QGridLayout* currentGrid;
};
