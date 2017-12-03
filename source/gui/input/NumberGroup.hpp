#pragma once
#include "bow/document/Document.hpp"
#include <QtWidgets>

class InputData;

class NumberGroup: public QGroupBox
{
public:
    NumberGroup(const QString& title);
    void addRow(const QString& name, const QString& unit, DocumentItem<double>& item);
    void addRow(const QString& name, const QString& unit, DocumentItem<int>& item);

private:
    QGridLayout* grid;

    void addRow(const QString& name, const QString& unit, QWidget* field);
};
