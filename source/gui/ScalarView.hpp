#pragma once
#include "Document.hpp"

#include <boost/lexical_cast.hpp>
#include <QtWidgets>

#include <functional>

// Todo: Check domain, domain perhaps as template parameter
template<typename T>
class ScalarView: public QLineEdit
{
public:
    ScalarView(DocumentItem<T> item)
        : doc_item(item)
    {
        // Update document
        QObject::connect(this, &QLineEdit::textEdited, this, &ScalarView::updateDocument);

        // Update view when document changes
        doc_item.connect([&]()
        {
            // Todo: This condition is a workaround to prevent the update from the document immediately overwriting
            // things the user put in, like a trailing dot for example. Better would be if the document only updates
            // all views but the one who sent the change.
            if(this->hasFocus())
                return;

            updateView();
        });

        // Update view when editing is finished to overwrite possible invalid input
        QObject::connect(this, &QLineEdit::editingFinished, this, &ScalarView::updateView);
    }

private:
    DocumentItem<T> doc_item;

    // Replace decimal comma with dot
    virtual void keyPressEvent(QKeyEvent *event) override
    {
        if(event->text() == QLocale().decimalPoint())
        {
            event->ignore();
            this->insert(".");
        }
        else
        {
            QLineEdit::keyPressEvent(event);
        }
    }

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
        this->setText(QString::number(doc_item.getData(), 'g', 10));   // Todo: Check domain // Todo: Magic number
        this->setPalette(QPalette());
    }
};
