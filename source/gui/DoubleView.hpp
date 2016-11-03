#pragma once
#include "../model/Document.hpp"
#include "../numerics/Domain.hpp"
#include <QtWidgets>

class DoubleView: public QLineEdit
{
public:
    DoubleView(DocItem<double>& doc_item)
        : doc_item(doc_item)
    {
        connection = doc_item.connect([this](const double& value)
        {
            setValue(value);
        });

        QObject::connect(this, &QLineEdit::editingFinished, [this]()
        {
            try
            {
                double value = getValue();
                this->doc_item = value;
            }
            catch(const std::runtime_error&)
            {
                setValue(this->doc_item);
            }
        });
    }

private:
    DocItem<double>& doc_item;
    Connection connection;

    void setValue(double value)
    {
        this->setText(QLocale::c().toString(value, 'g', 15));    // Todo: Magic number
    }

    double getValue() const
    {
        bool ok;
        double value = QLocale::c().toDouble(this->text(), &ok);

        if(!ok)
        {
            throw std::runtime_error("Cannot convert inout to number");
        }

        return value;
    }

    /*
    virtual void keyPressEvent(QKeyEvent* event) override
    {
        // Replace the local decimal separator with the one from the C locale (dot)
        if(event->text() == QLocale().decimalPoint())
        {
            insert(QLocale::c().decimalPoint());
        }
        else
        {
            QLineEdit::keyPressEvent(event);
        }
    }
    */
};
