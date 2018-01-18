#pragma once
#include <QtWidgets>
#include "numerics/Series.hpp"

class TableItem: public QTableWidgetItem
{
    virtual void setData(int role, const QVariant& data)
    {
        bool valid;
        double new_value = data.toDouble(&valid);

        if(valid || data.toString().isEmpty())
            QTableWidgetItem::setData(role, data);
    }
};

class TableWidget: public QTableWidget
{
public:
    TableWidget(const QString& x_label, const QString& y_label, int rows)
        : QTableWidget(rows, 2)
    {
        this->setHorizontalHeaderLabels({x_label, y_label});
        this->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        this->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
        this->verticalHeader()->sectionResizeMode(QHeaderView::Fixed);
        this->verticalHeader()->setDefaultSectionSize(horizontalHeader()->height());    // Todo: Better way?
        this->verticalHeader()->hide();

        for(int i = 0; i < rowCount(); ++i)
        {
            for(int j = 0; j < columnCount(); ++j)
                this->setItem(i, j, new TableItem());
        }
    }

    Series getData() const
    {
        Series data;
        for(int i = 0; i < rowCount(); ++i)
        {
            bool arg_valid, val_valid;
            double arg = QLocale::c().toDouble(this->item(i, 0)->text(), &arg_valid);
            double val = QLocale::c().toDouble(this->item(i, 1)->text(), &val_valid);

            if(arg_valid && val_valid)
                data.push_back(arg, val);
        }

        return data;
    }

    void setData(const Series& data)
    {
        for(int i = 0; i < rowCount(); ++i)
        {
            if(i < data.size())
            {
                this->item(i, 0)->setText(QLocale::c().toString(data.arg(i), 'g'));
                this->item(i, 1)->setText(QLocale::c().toString(data.val(i), 'g'));
            }
            else
            {
                this->item(i, 0)->setText("");
                this->item(i, 1)->setText("");
            }
        }
    }
};
