#pragma once
#include "model/document/Document.hpp"
#include <QtWidgets>

class DoubleView: public QLineEdit
{
public:
    DoubleView(DocumentItem<double>& doc_item);

private:
    DocumentItem<double>& doc_item;

    void update();
    void setValue(double value);
    void getValue(bool correct);
};
