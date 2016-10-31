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
