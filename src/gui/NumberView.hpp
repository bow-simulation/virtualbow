#pragma once
#include "Document.hpp"

#include <boost/lexical_cast.hpp>

#include <QtWidgets>

// Todo: Check domain, domain perhaps as template parameter
template<typename T>
class NumberView: public QLineEdit
{
public:
    NumberView(DocumentItem<T> item)
        : doc_item(item)
    {
        // Update view
        doc_item.connect([&]()
        {
            this->setText(QString::number(doc_item.getData()));   // Todo: Check domain
        });

        // Update document
        QObject::connect(this, &QLineEdit::editingFinished, [&]()
        {
            if(!this->isModified())
                return;

            bool old_state = this->blockSignals(true);

            try
            {
                T new_data = boost::lexical_cast<T>(this->text().toStdString());
                doc_item.setData(new_data);                // Todo: Check domain
            }
            catch(const boost::bad_lexical_cast&)
            {
                auto pick = QMessageBox::warning(this, "Warning", "Input is invalid",
                                                 QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Ok);

                switch(pick)
                {
                case QMessageBox::Ok:       // Leave input, mark it for easy deletion
                    this->setFocus();
                    this->selectAll();
                    break;

                case QMessageBox::Cancel:   // Reset input to last valid value
                    this->setText(QString::number(doc_item.getData()));
                    break;
                }
            }

            this->blockSignals(old_state);
        });
    }

private:
    DocumentItem<T> doc_item;
};
