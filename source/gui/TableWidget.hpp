#pragma once
#include <QtWidgets>

class TableItem: public QTableWidgetItem
{
    double old_value = 0.0;
    virtual void setData(int role, const QVariant& data)
    {
        bool valid;
        double new_value = data.toDouble(&valid);

        if(valid)
        {
            old_value = new_value;
            QTableWidgetItem::setData(role, data);
        }
        else
        {
            QTableWidgetItem::setData(role, old_value);
        }
    }
};

class TableWidget: public QTableWidget
{
public:
    TableWidget(const QStringList& labels)
        : QTableWidget(0, labels.size())
    {
        setHorizontalHeaderLabels(labels);
        horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
        verticalHeader()->sectionResizeMode(QHeaderView::Fixed);
        verticalHeader()->setDefaultSectionSize(horizontalHeader()->height());    // Todo: Better way?
        verticalHeader()->hide();
    }

    double getValue(int row, int col) const
    {
        QLocale::c().toDouble(item(row, col)->text());
    }

    void setValue(int row, int col, double value)
    {
        if(row >= rowCount())
            addRows(row - rowCount() + 1);

        if(col >= columnCount())
            addCols(col - columnCount() + 1);

        item(row, col)->setText(QLocale::c().toString(value, 'g'));
    }

private:
    void addRows(int n)
    {
        int old_rows = rowCount();
        setRowCount(old_rows + n);

        for(int i = old_rows; i < rowCount(); ++i)
        {
            for(int j = 0; j < columnCount(); ++j)
                setItem(i, j, new TableItem());
        }
    }

    void addCols(int n)
    {
        int old_cols = columnCount();
        setColumnCount(old_cols + n);

        for(int i = 0; i < rowCount(); ++i)
        {
            for(int j = old_cols; j < columnCount(); ++j)
                setItem(i, j, new TableItem());
        }
    }
};

