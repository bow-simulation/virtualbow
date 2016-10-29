#pragma once
#include "Document.hpp"

#include <QtWidgets>

class TextView: public QTextEdit
{
public:
    TextView(DocumentItem<std::string> doc_item)
        : doc_item(doc_item)
    {
        doc_item.connect([this]()
        {
            // Don't receive updates from document while the widget still has focus.
            // Todo: This is a workaround to prevent the update from the document immediately overwriting
            // things the user put in, like a trailing dot for example. Better would be if the document updated
            // all views except the one that was responsible for the change.
            if(this->hasFocus())
               return;

            this->setText(QString::fromStdString(this->doc_item.getData()));
        });
    }

private:
    DocumentItem<std::string> doc_item;
};
