#pragma once
#include "../../model/Document.hpp"
#include <QtWidgets>

class IntegerView: public QSpinBox
{
public:
    IntegerView(DocItem<int>& doc_item);

private:
    DocItem<int>& doc_item;
    Connection connection;
};
