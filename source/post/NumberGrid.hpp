#pragma once
#include <QtWidgets>

class NumberGrid : public QWidget
{
public:
    NumberGrid();

    void addColumn();
    void addGroup(const QString& name);
    void addValue(const QString& name, double value);

private:
    QHBoxLayout* columnLayout;
    QVBoxLayout* currentColumn;
    QGridLayout* currentGrid;
};