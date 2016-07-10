#pragma once
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

// Parameters: x_in, y_in, x_out, y_out
typedef std::function<void(const std::vector<double>&,
                           const std::vector<double>&,
                           std::vector<double>&,
                           std::vector<double>&)> DataFunction;

// Todo: Doesn't allow emptying a cell
class Delegate : public QItemDelegate
{
public:
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override
    {
        QLineEdit* line_edit = new QLineEdit(parent);
        line_edit->setValidator(new QDoubleValidator);  // Todo: Domain?
        return line_edit;
    }
};

class CurveEditor: public QDialog
{
    Q_OBJECT

public:
    CurveEditor(QWidget* parent, DataFunction f)
        : QDialog(parent),
          getOutputData(f)
    {
        // Init table
        table = new QTableWidget(20, 2);    // Todo: Magic number
        table->verticalHeader()->hide();
        //table->setItemDelegate(new Delegate);
        table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        //table->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        connect(table, SIGNAL(cellChanged(int, int)), this, SLOT(updateChart()));

        // Init chart
        QLineSeries *series = new QLineSeries();
        chart = new QChart();
        chart->legend()->hide();
        chart->addSeries(series);
        chart->createDefaultAxes();

        QChartView *view = new QChartView(chart);
        view->setRenderHint(QPainter::Antialiasing);
        view->setRubberBand(QChartView::RectangleRubberBand);

        // Ok and Cancel buttons
        QDialogButtonBox* btbox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        connect(btbox, &QDialogButtonBox::accepted, this, &QDialog::accept);
        connect(btbox, &QDialogButtonBox::rejected, this, &QDialog::reject);

        // Layout
        QHBoxLayout* h = new QHBoxLayout;
        h->addWidget(table);
        h->addWidget(view, 1);

        QVBoxLayout* v = new QVBoxLayout;
        v->addLayout(h);
        v->addWidget(btbox);
        this->setLayout(v);
        this->resize(900, 400);
    }

    void setInputLabels(const QString& lbx, const QString& lby)
    {
        table->setHorizontalHeaderLabels({{lbx, lby}});
    }

    void setOutputLabels(const QString& lbx, const QString& lby)
    {
        chart->axisX()->setTitleText(lbx);
        chart->axisY()->setTitleText(lby);
    }

    void getInputData(std::vector<double>& x, std::vector<double>& y)
    {
        for(int i = 0; i < table->rowCount(); ++i)
        {
            auto item_x = table->item(i, 0);
            auto item_y = table->item(i, 1);

            if(item_x == nullptr || item_y == nullptr)
                return;

            bool ok_x, ok_y;
            double x_val = item_x->text().toDouble(&ok_x);
            double y_val = item_y->text().toDouble(&ok_y);

            if(!ok_x || !ok_y)
                return;

            x.push_back(x_val);
            y.push_back(y_val);
        }

    }

    void setInputData(const std::vector<double>& x, const std::vector<double>& y)
    {
        assert(x.size() == y.size());

        // Todo: Is there a better way to set item texts without triggering a plot update every time?
        bool old_state = table->blockSignals(true);

        for(size_t i = 0; i < x.size(); ++i)
        {
           if(table->item(i, 0) == nullptr)
               table->setItem(i, 0,  new QTableWidgetItem);

           if(table->item(i, 1) == nullptr)
               table->setItem(i, 1,  new QTableWidgetItem);

           table->item(i, 0)->setText(QString::number(x[i]));
           table->item(i, 1)->setText(QString::number(y[i]));
        }

        table->blockSignals(old_state);
        updateChart();
    }

public slots:
    void updateChart()
    {
        // Get input and output data
        std::vector<double> x_in, y_in, x_out, y_out;
        getInputData(x_in, y_in);
        getOutputData(x_in, y_in, x_out, y_out);

        // Update chart
        QLineSeries *series = new QLineSeries();
        for(size_t i = 0; i < x_out.size(); ++i)
        {
            series->append(x_out[i], y_out[i]);
        }

        chart->removeSeries(chart->series()[0]);
        chart->addSeries(series);

        /*
        // Todo: Using clear() on existing series would be better, but the axes won't update properly.
        QLineSeries* series = dynamic_cast<QLineSeries*>(chart->series()[0]);
        series->clear();
        for(size_t i = 0; i < x_out.size(); ++i)
        {
            series->append(x_out[i], y_out[i]);
        }

        // ...
        // Alternative zu append:
        // series->replace(xy_out);
        */
    }

private:
    QTableWidget* table;
    QChart* chart;
    DataFunction getOutputData;
};
