#pragma once
#include "../model/Document.hpp"

#include <QtWidgets>

class StringView: public QTextEdit
{
public:
    StringView(DocItem<std::string>& doc_item)
        : doc_item(doc_item)
    {
        connection = doc_item.connect([this](const std::string& text)
        {
            // Don't receive updates from document while the widget still has focus.
            // Todo: This is a workaround to prevent the update from the document immediately overwriting
            // things the user put in, like a trailing dot for example. Better would be if the document updated
            // all views except the one that was responsible for the change.
            if(this->hasFocus())
               return;

            this->setText(QString::fromStdString(text));
        });

        QObject::connect(this, &QTextEdit::textChanged, [this]()
        {
            this->doc_item = this->toPlainText().toStdString();
        });
    }

private:
    DocItem<std::string>& doc_item;
    DocItem<std::string>::Connection connection;
};
