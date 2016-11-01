#pragma once
#include "../model/Document.hpp"
#include "../numerics/Series.hpp"
#include <QtWidgets>

class SeriesView: public QWidget
{
public:
    SeriesView(const QString& lb_args, const QString& lb_vals, DocItem<Series>& doc_item)
        : doc_item(doc_item)
    {
        // Widgets and Layout

        auto vbox = new QVBoxLayout();
        this->setLayout(vbox);

        table = new QTableWidget(1, 2);
        table->setHorizontalHeaderLabels({{lb_args, lb_vals}});
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        table->verticalHeader()->sectionResizeMode(QHeaderView::Fixed);
        table->verticalHeader()->setDefaultSectionSize(table->horizontalHeader()->height());    // Todo: Better way?
        table->verticalHeader()->hide();
        vbox->addWidget(table);

        auto bt_plus = new QPushButton(QIcon(":/list-add"), "Add");
        auto bt_minus = new QPushButton(QIcon(":/list-remove"), "Remove");

        auto hbox = new QHBoxLayout();
        hbox->addWidget(bt_plus);
        hbox->addWidget(bt_minus);
        vbox->addLayout(hbox);

        // Event handling

        connection = doc_item.connect([this](const Series& series)
        {
            setData(series);
        });

        QObject::connect(table, &QTableWidget::cellChanged, [this](int i, int j)
        {
            if(!table->item(i, j)->isSelected())    // Make sure the cell was changed by the user
                return;

            try
            {
                double value = getCellValue(i, j);
                Series series = this->doc_item;

                if(j == 0)
                    series.arg(i) = value;
                else if(j == 1)
                    series.val(i) = value;

                this->doc_item = series;
            }
            catch(const std::runtime_error&)
            {
                const Series& series = this->doc_item;

                if(j == 0)
                    setCellValue(i, j, series.arg(i));
                else if(j == 1)
                    setCellValue(i, j, series.val(i));
            }
        });

        QObject::connect(bt_plus, &QPushButton::clicked, [this]()
        {
             Series series = this->doc_item;
             series.add(0.0, 0.0);  // Todo: Magic numbers
             this->doc_item = series;
        });

        QObject::connect(bt_minus, &QPushButton::clicked, [this]()
        {
             Series series = this->doc_item;
             series.remove();
             this->doc_item = series;
        });
    }

private:
    DocItem<Series>& doc_item;
    DocItem<Series>::Connection connection;
    QTableWidget* table;

    void setData(const Series& series)
    {
        table->setRowCount(series.size());

        for(size_t i = 0; i < series.size(); ++i)
        {
            setCellValue(i, 0, series.arg(i));
            setCellValue(i, 1, series.val(i));
        }
    }

    void setCellValue(int i, int j, double value)
    {
        if(table->item(i, j) == nullptr)
            table->setItem(i, j,  new QTableWidgetItem);

        table->item(i, j)->setText(QLocale::c().toString(value, 'g', 15));    // Todo: Magic number
    }

    double getCellValue(int i, int j) const
    {
        bool ok;
        double value = QLocale::c().toDouble(table->item(i, j)->text(), &ok);

        if(!ok)
        {
            throw std::runtime_error("Cannot convert inout to number");
        }

        return value;
    }
};

/*
class SeriesView: public QTableWidget
{
public:
    SeriesView(const QString& lb_args, const QString& lb_vals, DocItem<Series>& doc_item)
        : QTableWidget(1, 2),
          doc_item(doc_item)
    {
        this->setHorizontalHeaderLabels({{lb_args, lb_vals}});
        this->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        this->verticalHeader()->sectionResizeMode(QHeaderView::Fixed);
        this->verticalHeader()->setDefaultSectionSize(this->horizontalHeader()->height());
        this->verticalHeader()->hide();

        connection = doc_item.connect([this](const Series& series)
        {
            setData(series);
        });

        QObject::connect(this, &QTableWidget::cellChanged, []()
        {
            qInfo() << "Cell changed";
        });

        QObject::connect(this, &QLineEdit::editingFinished, [this]()
        {
            bool ok;
            double value = getValue(&ok);

            if(ok)
            {
                try
                {
                    this->doc_item = value;
                }
                catch(const std::runtime_error&)
                {
                    setValue(this->doc_item);
                }
            }
            else
            {
                setValue(this->doc_item);
            }
        });
    }

private:
    DocItem<Series>& doc_item;
    DocItem<Series>::Connection connection;

    void focusOutEvent(QFocusEvent* event) override
    {
        setData(doc_item);
    }

    void setData(const Series& series)
    {
        this->setRowCount(series.size() + 1);

        for(size_t i = 0; i < series.size(); ++i)
        {
            if(this->item(i, 0) == nullptr)
                this->setItem(i, 0,  new QTableWidgetItem);

            if(this->item(i, 1) == nullptr)
                this->setItem(i, 1,  new QTableWidgetItem);

            this->item(i, 0)->setText(QLocale::c().toString(series.arg(i), 'g', 15));    // Todo: Magic number
            this->item(i, 1)->setText(QLocale::c().toString(series.val(i), 'g', 15));    // Todo: Magic number
        }

        this->setItem(series.size(), 0, nullptr);
        this->setItem(series.size(), 1, nullptr);
    }

};
*/
