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
        : View<T>(doc, view_function)
    {
        View<T>::updateView();
        connect(this, &QLineEdit::editingFinished, [&](){ View<T>::updateDoc(); });
    }

    virtual void updateDoc(T& data) override
    {
        try
        {
            T new_data = boost::lexical_cast<T>(this->text().toStdString());
            data = new_data; // Todo: Check domain
        }
        catch(...)  // Todo
        {
            bool old_state = this->blockSignals(true);

            QMessageBox::critical(this, "Error", "Invalid input");  // Todo: Ok: Mark wrong input, Cancel: Restore old value.
            this->setText(QString::number(data));
            this->setFocus();

            this->blockSignals(old_state);
        }
    }

    virtual void updateView(const T& data) override
    {
        this->setText(QString::number(data));   // Todo: Check domain
    }
};

/*
class DoubleView: public QLineEdit, public View<double>
{
    //Q_OBJECT

public:
    DoubleView(Document& doc, ViewFunction<double> view_fn)
        : View<double>(doc, view_fn)
    {
        View<double>::updateView();
        connect(this, &QLineEdit::editingFinished, [&](){ View<double>::updateDoc(); });
    }

    virtual void updateDoc(double& data) override
    {
        try
        {
            double new_data = boost::lexical_cast<double>(this->text().toStdString());
            data = new_data; // Todo: Check domain
        }
        catch(...)  // Todo
        {
            bool old_state = this->blockSignals(true);

            QMessageBox::critical(this, "Error", "Invalid input");  // Todo: Ok: Mark wrong input, Cancel: Restore old value.
            this->setText(QString::number(data));
            this->setFocus();

            this->blockSignals(old_state);
        }
    }

    virtual void updateView(const double& data) override
    {
        this->setText(QString::number(data));   // Todo: Check domain
    }
};
*/
