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

        auto bt_plus = new QPushButton("Insert");
        auto bt_minus = new QPushButton("Remove");

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
    Connection connection;
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
