#pragma once
#include "bow/document/Document.hpp"
#include <QtWidgets>

class IntegerView: public QSpinBox
{
public:
    IntegerView(DocumentItem<int>& doc_item);

private:
    DocumentItem<int>& doc_item;
    void update_value();
    void update_error();
};
