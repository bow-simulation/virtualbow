#pragma once
#include "model/document/Document.hpp"
#include <QtWidgets>

class IntegerView: public QSpinBox
{
public:
    IntegerView(DocumentItem<int>& doc_item);

private:
    DocumentItem<int>& doc_item;
};
