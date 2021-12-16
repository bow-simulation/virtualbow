#pragma once
#include <QWidget>

class UnitGroup;
class QHBoxLayout;
class QVBoxLayout;
class QGridLayout;

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
