#pragma once
#include "Document.hpp"
#include "../numerics/Domain.hpp"

#include <QtWidgets>

template<DomainTag D>
class DoubleValidator : public QDoubleValidator
{
public:
    DoubleValidator(DocumentItem<double> doc_item)
        : QDoubleValidator(Domain<D>::min(), Domain<D>::max(), 10),     // Todo: Magic number
          doc_item(doc_item)
    {
        this->setLocale(QLocale(QLocale::C));
    }

    virtual State validate(QString& input, int& pos) const override
    {
        State state = QDoubleValidator::validate(input, pos);
        if(state == Acceptable)
        {
            doc_item.setData(this->locale().toDouble(input));
        }

        return state;
    }

    virtual void fixup(QString& input) const override
    {
        input = locale().toString(doc_item.getData(), 'g', decimals());
    }

private:
    mutable DocumentItem<double> doc_item;  // Todo: Not so pretty
};

template<DomainTag D>
class DoubleView: public QLineEdit
{
public:
    DoubleView(DocumentItem<double> item): doc_item(item)
    {
        this->setValidator(new DoubleValidator<D>(doc_item));   // Todo: DocumentItem by reference? (here and elsewhere)

        doc_item.connect([&]()
        {
            // Don't receive updates from document while the widget still has focus.
            // Todo: This is a workaround to prevent the update from the document immediately overwriting
            // things the user put in, like a trailing dot for example. Better would be if the document updated
            // all views except the one that was responsible for the change.
            if(this->hasFocus())
               return;

            // Update view with new value, exception if out of bounds
            double new_value = doc_item.getData();
            if(!Domain<D>::contains(new_value))
            {
                throw std::runtime_error("Value lies outside valid domain");
            }

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
