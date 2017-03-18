#pragma once
#include "model/Document.hpp"
#include <QtWidgets>

class InputData;

class NumberGroup: public QGroupBox
{
public:
    NumberGroup(const QString& title);
    void addRow(const QString& name, const QString& unit, DocItem<double>& item);
    void addRow(const QString& name, const QString& unit, DocItem<int>& item);

private:
    QGridLayout* grid;

    void addRow(const QString& name, const QString& unit, QWidget* field);
};
