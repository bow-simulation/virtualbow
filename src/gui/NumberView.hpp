#pragma once
#include "../numerics/Domain.hpp"
#include "../numerics/Units.hpp"
#include "Document.hpp"

#include <boost/lexical_cast.hpp>

#include <QtWidgets>

template<typename T, Domain D = Domain::All>
class NumberView: public QLineEdit
{
public:
    NumberView(DocumentItem<T> item)
        : doc_item(item)
    {
        // Update view
        doc_item.connect([&]()
        {
            this->setText(QString::number(doc_item.getData()));   // Todo: Check domain
        });

        // Update document
        QObject::connect(this, &QLineEdit::editingFinished, [&]()
        {
            try
            {
                T new_data = boost::lexical_cast<T>(this->text().toStdString());
                doc_item.setData(new_data); // Todo: Check domain
            }
            catch(...)  // Todo
            {
                bool old_state = this->blockSignals(true);

                QMessageBox::critical(this, "Error", "Invalid input");  // Todo: Ok: Mark wrong input, Cancel: Restore old value.
                this->setText(QString::number(doc_item.getData()));
                this->setFocus();

                this->blockSignals(old_state);
            }
        });
    }

private:
    DocumentItem<T> doc_item;

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
