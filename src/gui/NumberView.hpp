#pragma once
#include "../numerics/Domain.hpp"
#include "../numerics/Units.hpp"
#include "Document.hpp"

#include <boost/lexical_cast.hpp>

#include <QtWidgets>

template<typename T, Domain D>
class NumberView: public QLineEdit, public View<T>
{
public:
    NumberView(Document& doc, ViewFunction<T> view_function)
        : View<T>(view_function)
    {
        this->setDocument(doc);

        connect(this, &QLineEdit::editingFinished, [&]()
        {
            try
            {
                T new_data = boost::lexical_cast<T>(this->text().toStdString());
                this->getData() = new_data; // Todo: Check domain
            }
            catch(...)  // Todo
            {
                bool old_state = this->blockSignals(true);

                this->setText(QString::number(this->getConstData()));
                QMessageBox::critical(this, "Error", "Invalid input");  // Todo: Ok: Mark wrong input, Cancel: Restore old value.
                this->setFocus();

                this->blockSignals(old_state);
            }
        });
    }

    virtual void update()
    {
        this->setText(QString::number(this->getConstData()));   // Todo: Check domain
    }
};
