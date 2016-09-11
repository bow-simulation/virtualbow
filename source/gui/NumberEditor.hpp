#pragma once
#include "Document.hpp"
#include "../numerics/Domain.hpp"

#include <boost/lexical_cast.hpp>
#include <QtWidgets>
#include <wobjectdefs.h>

#include <functional>

template<Domain D>
class NumberEditor: public QLineEdit
{
    W_OBJECT(NumberEditor)

public:
    NumberEditor()
    {
        QObject::connect(this, &QLineEdit::textEdited, this, &NumberEditor::updateValue);
        QObject::connect(this, &QLineEdit::editingFinished, this, &NumberEditor::updateDisplay);

        setValue(DomainInfo<D>::default_value());
    }

    void setValue(double new_value)
    {
        if(DomainInfo<D>::contains(new_value))
        {
            value = new_value;
            updateDisplay();
            emit valueChanged(new_value);
        }
    }

    double getValue() const
    {
        return value;
    }

signals:
    void valueChanged(double new_value)
    W_SIGNAL(valueChanged, new_value)

private:
    double value;

    virtual void keyPressEvent(QKeyEvent *event) override
    {
        // Replace the local decimal separator with a dot
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

    void updateValue()
    {
        bool success;
        double new_value = this->text().toDouble(&success);

        if(success && DomainInfo<D>::contains(new_value))
        {
            value = new_value;
            this->setPalette(QPalette());
            emit valueChanged(new_value);
        }
        else
        {
            QPalette palette;
            palette.setColor(QPalette::Base, QColor(255, 128, 128));
            this->setPalette(palette);
        }


        /*
        try
        {


            double new_value = boost::lexical_cast<double>(this->text().toStdString());

            if(DomainInfo<D>::contains(new_value))
            {

            }

            value = new_value;  // Todo: Check domain
            this->setPalette(QPalette());
            emit valueChanged(new_value);
        }
        catch(const boost::bad_lexical_cast&)
        {
            QPalette palette;
            palette.setColor(QPalette::Base, QColor(255, 128, 128));
            this->setPalette(palette);
        }
        */
    }

    void updateDisplay()
    {
        this->setText(QString::number(value, 'g', 10));   // Todo: Check domain // Todo: Magic number
        this->setPalette(QPalette());
    }
};

#include <wobjectimpl.h>

W_OBJECT_IMPL(NumberEditor<D>, template<Domain D>)
