#pragma once
#include "Document.hpp"

#include <boost/lexical_cast.hpp>
#include <QtWidgets>

#include <functional>

// Todo: Check domain, domain perhaps as template parameter
template<typename T>
class NumberView: public QLineEdit
{
public:
    NumberView(DocumentItem<T> item)
        : doc_item(item)
    {
        // Update document
        QObject::connect(this, &QLineEdit::textEdited, this, &NumberView::updateDocument);

        // Update view
        doc_item.connect([&](){ updateView(); });    // Todo: Use std::bind
        QObject::connect(this, &QLineEdit::editingFinished, this, &NumberView::updateView);
    }

private:
    DocumentItem<T> doc_item;

    void updateDocument()
    {
        try
        {
            T new_data = boost::lexical_cast<T>(this->text().toStdString());
            doc_item.setData(new_data);                // Todo: Check domain

            this->setPalette(QPalette());
        }
        catch(const boost::bad_lexical_cast&)
        {
            QPalette palette;
            palette.setColor(QPalette::Base, QColor(255, 128, 128));
            this->setPalette(palette);
        }
    }

    void updateView()
    {
        // Todo: This condition is a workaround to prevent the update from the document immediately overwriting
        // things the user put in, like a trailing dot for example. Better would be if the document only updates
        // all views but the one who sent the change.
        if(this->hasFocus())
            return;

        this->setText(QString::number(doc_item.getData(), 'g', 10));   // Todo: Check domain
        this->setPalette(QPalette());
    }
};
