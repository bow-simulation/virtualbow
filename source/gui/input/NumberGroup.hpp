#pragma once
#include "../../model/Document.hpp"
#include <QtWidgets>

class InputData;

class NumberGroup: public QGroupBox
{
public:
    NumberGroup(InputData& data, const QString& title);
    void addRow(const QString& lb, DocItem<double>& item);
    void addRow(const QString& lb, DocItem<int>& item);

private:
    QVBoxLayout* vbox;

    void addRow(const QString& lb, QWidget* field);
};
