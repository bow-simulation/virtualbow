#pragma once
#include "Document.hpp"
#include "../numerics/Domain.hpp"

#include <boost/lexical_cast.hpp>
#include <QtWidgets>
#include <wobjectdefs.h>

template<DomainTag D>
class DoubleEditor: public QLineEdit
{
    W_OBJECT(DoubleEditor)

public:
    DoubleEditor(QWidget* parent = nullptr): QLineEdit(parent)
    {
        QObject::connect(this, &QLineEdit::textEdited, this, &DoubleEditor::updateValue);
        QObject::connect(this, &QLineEdit::editingFinished, this, &DoubleEditor::updateDisplay);

        setValue(Domain<D>::default_value());
    }

    void setValue(double new_value)
    {
        if(Domain<D>::contains(new_value))
        {
            value = new_value;
            updateDisplay();
            emit valueChanged(new_value);
        }
        else
        {
            throw std::runtime_error("Value is not in the required domain");
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

        if(success && Domain<D>::contains(new_value))
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
    }

    void updateDisplay()
    {
        this->setText(QString::number(value, 'g', 10));   // Todo: Check domain // Todo: Magic number
        this->setPalette(QPalette());
    }
};

#include <wobjectimpl.h>

W_OBJECT_IMPL(DoubleEditor<D>, template<DomainTag D>)
