#pragma once
#include "../model/Document.hpp"
#include "../numerics/Domain.hpp"

#include <QtWidgets>

template<DomainTag D>
class DoubleValidator : public QDoubleValidator
{
public:
    DoubleValidator(DocItem<double>& doc_item)
        : QDoubleValidator(Domain<D>::min(), Domain<D>::max(), 15),     // Todo: Magic number
          doc_item(doc_item)
    {
        this->setLocale(QLocale(QLocale::C));
    }

    virtual State validate(QString& input, int& pos) const override
    {
        State state = QDoubleValidator::validate(input, pos);
        if(state == Acceptable)
        {
            double new_value = locale().toDouble(input);
            if(new_value != doc_item)     // Todo: Move this check to DocumentItem?
            {
                doc_item = new_value;
            }
        }

        return state;
    }

    virtual void fixup(QString& input) const override
    {
        if(!Domain<D>::contains(doc_item))
        {
            throw std::runtime_error("Value lies outside of valid domain");
        }

        input = locale().toString(doc_item, 'g', decimals());
    }

private:
    DocItem<double>& doc_item;
};

template<DomainTag D>
class DoubleView: public QLineEdit
{
public:
    DoubleView(DocItem<double>& doc_item): doc_item(doc_item)
    {
        this->setValidator(new DoubleValidator<D>(doc_item));
        connection = doc_item.connect([this](const double&)
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
    DocItem<double>& doc_item;
    DocItem<double>::Connection connection;

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
