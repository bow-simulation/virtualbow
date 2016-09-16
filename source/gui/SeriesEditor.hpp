#pragma once
#include "../numerics/DataSeries.hpp"
#include "Plot.hpp"

#include <QtWidgets>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
QT_CHARTS_USE_NAMESPACE

#include <functional>
#include <iostream>
#include <cassert>
#include <vector>

// Todo: Why do the axis not rescale on updating?
// Todo: Make sure plot always includes (0, 0) after updates.
// Todo: Allow arbitrary number of lines in the table, resize dynamically.
// Todo: Validate input via QItemDelegate and QDoubleValidator. Problem: How to allow emptying a cell?
// Todo: Better policy than just stop reading data on the first line that isn't a number.
// Todo: Show control points.
// Todo: Enable vertical scroll bar on table without it covering parts of the cells.
// Todo: Enable copy and paste in table and deleting cells without double clicking on them first.
// Todo: Scrolling, panning on the plot

// Todo: Doesn't allow emptying a cell
class Delegate : public QItemDelegate
{
public:
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& /*option*/, const QModelIndex& /*index*/) const override
    {
        QLineEdit* line_edit = new QLineEdit(parent);
        line_edit->setValidator(new QDoubleValidator);  // Todo: Domain?
        return line_edit;
    }
};

class SeriesEditor: public QDialog
{
    Q_OBJECT

public:
    typedef std::function<DataSeries(const DataSeries&)> Transform;

    SeriesEditor(QWidget* parent, Transform transform)
        : QDialog(parent),
          transform(transform)
    {
        // Init table
        table = new QTableWidget(20, 2);    // Todo: Magic number
        table->verticalHeader()->hide();
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        QObject::connect(table, &QTableWidget::cellChanged, this, &SeriesEditor::updateChart);

        // Init plot
        plot = new Plot();
        plot->addSeries();

        QChartView *view = new QChartView(plot);
        view->setRenderHint(QPainter::Antialiasing);

        // Ok and Cancel buttons
        btbox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        QObject::connect(btbox, &QDialogButtonBox::rejected, this, &QDialog::reject);
        QObject::connect(btbox, &QDialogButtonBox::accepted, this, &QDialog::accept);

        // Layout
        QHBoxLayout* h = new QHBoxLayout;
        h->addWidget(table);
        h->addWidget(view, 1);

        QVBoxLayout* v = new QVBoxLayout;
        v->addLayout(h);
        v->addWidget(btbox);

        this->setLayout(v);
        this->resize(900, 400);

        setData(DataSeries());
    }

    void setInputLabels(const QString& lbx, const QString& lby)
    {
        table->setHorizontalHeaderLabels({{lbx, lby}});
    }

    void setOutputLabels(const QString& lbx, const QString& lby)
    {
        plot->axisX()->setTitleText(lbx);
        plot->axisY()->setTitleText(lby);
    }

    DataSeries getData() const
    {
        DataSeries data;
        for(int i = 0; i < table->rowCount(); ++i)
        {
            auto item_arg = table->item(i, 0);
            auto item_val = table->item(i, 1);

            if(item_arg == nullptr || item_val == nullptr)
                return data;

            bool ok_arg, ok_val;
            double arg = item_arg->text().toDouble(&ok_arg);
            double val = item_val->text().toDouble(&ok_val);

            if(!ok_arg || !ok_val)
                return data;

            data.add(arg, val);
        }

        return data;
    }

    void setData(DataSeries data)
    {
        bool old_state = table->blockSignals(true);     // Todo: Is there a better way to set item texts without triggering a plot update every time?

        for(size_t i = 0; i < data.size(); ++i)
        {
           if(table->item(i, 0) == nullptr)
               table->setItem(i, 0,  new QTableWidgetItem);

           if(table->item(i, 1) == nullptr)
               table->setItem(i, 1,  new QTableWidgetItem);

           table->item(i, 0)->setText(QString::number(data.arg(i)));
           table->item(i, 1)->setText(QString::number(data.val(i)));
        }

        table->blockSignals(old_state);
        updateChart();
    }

public slots:
    void updateChart()
    {
        DataSeries input = getData();
        DataSeries output = transform(input);

        plot->setData(0, output);
        btbox->button(QDialogButtonBox::Ok)->setEnabled(output.size() != 0);
    }

private:
    Transform transform;

    QTableWidget* table;
    Plot* plot;
    QDialogButtonBox* btbox;
};
