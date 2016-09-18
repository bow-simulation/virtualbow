#pragma once
#include "Document.hpp"
#include "../numerics/Domain.hpp"

#include <QtWidgets>

template<DomainTag D>
class DoubleValidator : public QDoubleValidator
{
public:
    DoubleValidator(DocumentItem<double> doc_item)
        : QDoubleValidator(Domain<D>::min(), Domain<D>::max(), 15),     // Todo: Magic number
          doc_item(doc_item)
    {
        this->setLocale(QLocale(QLocale::C));
    }

    virtual State validate(QString& input, int& pos) const override
    {
        qInfo() << "Validate!";

        State state = QDoubleValidator::validate(input, pos);
        if(state == Acceptable)
        {
            double new_value = locale().toDouble(input);
            if(new_value != doc_item.getData())     // Todo: Move this check to DocumentItem?
            {
                doc_item.setData(new_value);
            }
        }

        return state;
    }

    virtual void fixup(QString& input) const override
    {
        double value = doc_item.getData();
        if(!Domain<D>::contains(value))
        {
            throw std::runtime_error("Value lies outside of valid domain");
        }

        input = locale().toString(value, 'g', decimals());
    }

private:
    mutable DocumentItem<double> doc_item;  // Todo: Not so pretty
};

template<DomainTag D>
class DoubleView: public QLineEdit
{
public:
    DoubleView(DocumentItem<double> doc_item): doc_item(doc_item)
    {
        this->setValidator(new DoubleValidator<D>(doc_item));
        doc_item.connect([this]()
        {
            // Don't receive updates from document while the widget still has focus.
            // Todo: This is a workaround to prevent the update from the document immediately overwriting
            // things the user put in, like a trailing dot for example. Better would be if the document updated
            // all views except the one that was responsible for the change.
            if(this->hasFocus())
               return;

            QString text;
            this->validator()->fixup(text);
            this->setText(text);
        });
    }

private:
    DocumentItem<double> doc_item;

    // Replace the local decimal separator with the one from the C locale (dot)
    virtual void keyPressEvent(QKeyEvent *event) override
    {
        if(event->text() == QLocale().decimalPoint())
        {
            insert(validator()->locale().decimalPoint());
        }
        else
        {
            QLineEdit::keyPressEvent(event);
        }
    }
};
