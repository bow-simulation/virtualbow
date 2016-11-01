#pragma once
#include "qcustomplot/qcustomplot.h"

class Plot: public QCustomPlot
{
public:
    Plot()
    {
        /*
        QValueAxis *axisX = new QValueAxis;
        axisX->setLabelFormat("%g");
        //axisX->setTitleText(x_label);

        QValueAxis *axisY = new QValueAxis;
        axisY->setLabelFormat("%g");
        //axisY->setTitleText(y_label);

        //this->setTitle(title);
        this->addAxis(axisX, Qt::AlignBottom);
        this->addAxis(axisY, Qt::AlignLeft);
        this->legend()->hide();
        this->layout()->setContentsMargins(0, 0, 0, 0);
        this->setBackgroundRoundness(0);
        */
    }

    int addSeries(const Series& data, const QString& name = "")
    {
        /*
        int index = addSeries(name);
        setData(index, data);

        return index;
        */
    }

    int addSeries(const QString& name = "")
    {
        /*
        if(!name.isEmpty())
            this->legend()->show();

        QLineSeries* series = new QLineSeries();
        QChart::addSeries(series);

        series->setName(name);
        series->attachAxis(this->axisX());
        series->attachAxis(this->axisY());

        return this->series().size() - 1;
        */
    }

    void setData(int index, const Series& data)
    {
        /*
        auto* series = dynamic_cast<QLineSeries*>(this->series()[index]);
        series->clear();

        for(size_t i = 0; i < data.size(); ++i)
        {
            series->append(data.arg(i), data.val(i));
        }

        fit();
        */
    }

    void fit()
    {
        //qInfo() << "Fit!";
    }
};


/*
// Update chart
QLineSeries *series = new QLineSeries();
for(size_t i = 0; i < output.size(); ++i)
{
    series->append(output.arg(i), output.val(i));
}

chart->removeSeries(chart->series()[0]);
chart->addSeries(series);
*/
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
