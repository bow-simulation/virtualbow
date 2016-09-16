#pragma once
#include "Document.hpp"
#include "DoubleEditor.hpp"
#include "../numerics/Domain.hpp"

#include <QtWidgets>

template<DomainTag ArgDomain, DomainTag ValDomain>
class SeriesView: public QWidget
{
public:
    SeriesView(DocumentItem<DataSeries> item, const QString& arg_label, const QString& val_label)
        : doc_item(item)
    {
        table = new QTableWidget(0, 2);

        table->verticalHeader()->hide();
        table->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
        table->verticalHeader()->setDefaultSectionSize(table->horizontalHeader()->height());    // Todo: Better solution?

        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        table->setHorizontalHeaderLabels({{arg_label, val_label}});

        /*
        table->setCellWidget(0, 0, new NumberEditor<ArgDomain>());
        table->setCellWidget(0, 1, new NumberEditor<ValDomain>());

        auto edit = dynamic_cast<NumberEditor<ArgDomain>*>(table->cellWidget(0, 0));
        edit->setValue(3.1415926535);
        */

        // Update table data
        doc_item.connect([&]()
        {
            DataSeries data = doc_item.getData();

            int n_data = data.size();
            int n_table = table->rowCount();
            table->setRowCount(n_data);

            if(n_data > n_table)
            {
                for(int row = n_table; row < n_data; ++row)
                {
                    auto* edit_arg = new DoubleEditor<ArgDomain>();
                    auto* edit_val = new DoubleEditor<ValDomain>();
                    edit_arg->setValue(data.arg(row));
                    edit_val->setValue(data.val(row));
                    // Todo: Connect valueChanged event

                    table->setCellWidget(row, 0, edit_arg);
                    table->setCellWidget(row, 1, edit_val);
                }
            }
        });

        auto bt_add = new QPushButton(QIcon(":/list-add"), "");
        QObject::connect(bt_add, &QPushButton::clicked, [&]()
        {
            DataSeries data = doc_item.getData();
            data.add(Domain<ArgDomain>::default_value(), Domain<ValDomain>::default_value());
            doc_item.setData(data);
        });

        auto bt_remove = new QPushButton(QIcon(":/list-remove"), "");
        QObject::connect(bt_remove, &QPushButton::clicked, [&]()
        {
            DataSeries data = doc_item.getData();
            if(data.size() > 0)
            {
                data.remove();
                doc_item.setData(data);
            }
        });

        // Layout
        auto h = new QHBoxLayout();
        h->addWidget(bt_add);
        h->addWidget(bt_remove);

        auto v = new QVBoxLayout();
        v->addWidget(table);
        v->addLayout(h);

        this->setLayout(v);
    }

private:
    DocumentItem<DataSeries> doc_item;
    QTableWidget* table;

    template<DomainTag D>
    class ItemDelegate : public QItemDelegate
    {
    public:
        QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override
        {
            return new DoubleEditor<D>(parent);
        }
    };
};

/*
// Todo: Check domain, domain perhaps as template parameter
template<typename T>
class ScalarView: public QLineEdit
{
public:
    ScalarView(DocumentItem<T> item)
        : doc_item(item)
    {
        // Update document
        QObject::connect(this, &QLineEdit::textEdited, this, &ScalarView::updateDocument);

        // Update view when document changes
        doc_item.connect([&]()
        {
            // Todo: This condition is a workaround to prevent the update from the document immediately overwriting
            // things the user put in, like a trailing dot for example. Better would be if the document only updates
            // all views but the one who sent the change.
            if(this->hasFocus())
                return;

            updateView();
        });

        // Update view when editing is finished to overwrite possible invalid input
        QObject::connect(this, &QLineEdit::editingFinished, this, &ScalarView::updateView);
    }

private:
    DocumentItem<T> doc_item;

    // Replace decimal comma with dot
    virtual void keyPressEvent(QKeyEvent *event) override
    {
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

    void updateDocument()
    {
        try
        {
            T new_data = boost::lexical_cast<T>(this->text().toStdString());
            doc_item.setData(new_data);                // Todo: Check domain

            this->setPalette(QPalette());
        }
        catch(const boost::bad_lexical_cast&)
        {
            QPalette palette;
            palette.setColor(QPalette::Base, QColor(255, 128, 128));
            this->setPalette(palette);
        }
    }

    void updateView()
    {
        this->setText(QString::number(doc_item.getData(), 'g', 10));   // Todo: Check domain // Todo: Magic number
        this->setPalette(QPalette());
    }
};
*/
