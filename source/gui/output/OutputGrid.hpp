#pragma once
#include <QtWidgets>

class OutputGrid: public QWidget
{
public:
    OutputGrid();
    void add(int i, int j, const QString& text, double value);

private:
    QGridLayout* grid;
};
