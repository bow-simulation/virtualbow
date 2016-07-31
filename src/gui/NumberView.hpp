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
    NumberView(Document& document, ViewFunction<T> view_function)
        : View<T>(document, view_function)
    {
        View<T>::document.addView(this);

        connect(this, &QLineEdit::editingFinished, [&]()
        {
            try
            {
                T new_data = boost::lexical_cast<T>(this->text().toStdString());
                this->setData(new_data); // Todo: Check domain
            }
            catch(...)  // Todo
            {
                bool old_state = this->blockSignals(true);

                this->setText(QString::number(this->getData()));
                QMessageBox::critical(this, "Error", "Invalid input");  // Todo: Ok: Mark wrong input, Cancel: Restore old value.
                this->setFocus();

                this->blockSignals(old_state);
            }
        });
    }

    ~NumberView()
    {
        View<T>::document.removeView(this);
    }

    virtual void update() override
    {
        this->setText(QString::number(this->getData()));   // Todo: Check domain
    }
};

