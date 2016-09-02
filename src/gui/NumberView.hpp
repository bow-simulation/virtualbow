#pragma once
#include "../numerics/Domain.hpp"
#include "../numerics/Units.hpp"
#include "Document.hpp"

#include <boost/lexical_cast.hpp>

#include <QtWidgets>

template<typename T, Domain D = Domain::All>
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
            try
            {
                T new_data = boost::lexical_cast<T>(this->text().toStdString());
                doc_item.setData(new_data); // Todo: Check domain
            }
            catch(...)  // Todo
            {
                bool old_state = this->blockSignals(true);

                // Todo: Ok: Mark wrong input, Cancel: Restore old value
                // Todo: Better error messages
                QMessageBox::critical(this, "Error", "Invalid input");
                this->setText(QString::number(doc_item.getData()));
                this->setFocus();

                this->blockSignals(old_state);
            }
        });
    }

private:
    DocumentItem<T> doc_item;
};
