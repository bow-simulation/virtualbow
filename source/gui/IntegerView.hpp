#pragma once
#include "../model/Document.hpp"

#include <QtWidgets>

class IntegerView: public QSpinBox
{
public:
    IntegerView(DocItem<unsigned>& doc_item)
        : doc_item(doc_item)
    {
        this->setMinimum(1);

        connection = this->doc_item.connect([this](const unsigned& value)
        {
            // Don't receive updates from document while the widget still has focus.
            // Todo: This is a workaround to prevent the update from the document immediately overwriting
            // things the user put in, like a trailing dot for example. Better would be if the document updated
            // all views except the one that was responsible for the change.
            if(this->hasFocus())
               return;

            this->setValue(value);
        });

        QObject::connect(this, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this](int value)
        {
            this->doc_item = value;
        });
    }

private:
    DocItem<unsigned>& doc_item;
    DocItem<unsigned>::Connection connection;
};
