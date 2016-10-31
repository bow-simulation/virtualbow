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
            bool ok;
            double value = getValue(&ok);

            if(ok)
            {
                this->doc_item = value;
            }
            else
            {
                setValue(this->doc_item);
            }
        });
    }

private:
    DocItem<double>& doc_item;
    DocItem<double>::Connection connection;

    void setValue(double value)
    {
        this->setText(QLocale::c().toString(value, 'g', 15));    // Todo: Magic number
    }

    double getValue(bool* ok) const
    {
        return QLocale::c().toDouble(this->text(), ok);
    }

    // Replace the local decimal separator with the one from the C locale (dot)
    virtual void keyPressEvent(QKeyEvent *event) override
    {
        if(event->text() == QLocale().decimalPoint())
        {
            insert(QLocale::c().decimalPoint());
        }
        else
        {
            QLineEdit::keyPressEvent(event);
        }
    }
};
