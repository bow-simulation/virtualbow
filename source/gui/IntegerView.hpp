#pragma once
#include "../model/Document.hpp"

#include <QtWidgets>

class IntegerView: public QSpinBox
{
public:
    IntegerView(DocItem<int>& doc_item)
        : doc_item(doc_item)
    {
        connection = this->doc_item.connect([this](const unsigned& value)
        {
            this->setValue(value);
        });

        QObject::connect(this, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this](int value)
        {
            this->doc_item = value;
        });
    }

private:
    DocItem<int>& doc_item;
    DocItem<int>::Connection connection;
};
