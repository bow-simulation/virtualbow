#pragma once
#include "../numerics/Domain.hpp"
#include "../numerics/Units.hpp"

#include <boost/lexical_cast.hpp>

#include <QtWidgets>

template<typename T, Domain D>
class NumberView: public QLineEdit
{
public:
    NumberView(T& data): data(data)
    {
        this->setText(QString::number(data));   // Todo: Check domain

        connect(this, &QLineEdit::editingFinished, [&]()
        {
            try
            {
                T new_data = boost::lexical_cast<T>(this->text().toStdString());
                // Todo: Check domain
                data= new_data;
            }
            catch(...)  // Todo
            {
                bool old_state = this->blockSignals(true);

                this->setText(QString::number(data));
                QMessageBox::critical(this, "Error", "Invalid input");  // Todo: Ok: Mark wrong input, Cancel: Restore old value.
                this->setFocus();

                this->blockSignals(old_state);
            }
        });
    }

private:
    T& data;
};

