#pragma once
#include "gui/units/UnitSystem.hpp"
#include <QtWidgets>

class DoubleEditor: public QWidget
{
    Q_OBJECT

public:
    DoubleEditor(const QString& text);

    double getData() const;
    void setData(double data);
    void setUnit(const Unit& unit);

private:
    void update();

signals:
    void modified();

private:
    QLabel* text_label;
    QLabel* unit_label;
    QLineEdit* line_edit;
    bool changed;

    double data;
    Unit unit;
};
