#pragma once
#include "gui/viewmodel/units/UnitSystem.hpp"
#include <QWidget>

class QLabel;
class QLineEdit;

class DoubleEditor: public QWidget
{
    Q_OBJECT

public:
    DoubleEditor(const QString& text, const UnitGroup& group);

    double getData() const;
    void setData(double data);

signals:
    void modified();

private:
    void update();

private:
    QLabel* text_label;
    QLabel* unit_label;
    QLineEdit* line_edit;
    bool changed;

    double data;
    Unit unit;
};
