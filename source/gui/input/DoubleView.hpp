#pragma once
#include "../../model/Document.hpp"
#include <QtWidgets>

class DoubleView: public QLineEdit
{
public:
    DoubleView(DocItem<double>& doc_item);

private:
    DocItem<double>& doc_item;
    Connection connection;

    void setValue(double value);
    void getValue(bool correct);
};
