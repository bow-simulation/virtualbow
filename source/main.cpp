#include "gui/MainWindow.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QGuiApplication::setApplicationDisplayName("Bow Simulator");
    QGuiApplication::setApplicationVersion("0.1.0");
    QGuiApplication::setOrganizationDomain("https://bow-simulator.sourceforge.net");

    MainWindow window;
    window.show();

    return app.exec();
}

/*
#include <QtGui>
#include "gui/qcustomplot/qcustomplot.h"
#include <cmath>

void setup(QCustomPlot* customPlot)
{
    QVector<double> x, y;
    for(double t = 0.0; t < 2*M_PI; t += 0.01)
    {
        x.push_back(sin(t));
        y.push_back(cos(t));
    }

    QVector<double> x2, y2;
    for(double t = 0.0; t < 2*M_PI; t += 0.01)
    {
        x2.push_back(sin(t) + 1.0);
        y2.push_back(cos(t));
    }

    QCPCurve* newCurve = new QCPCurve(customPlot->xAxis, customPlot->yAxis);
    newCurve->setData(x, y);

    QCPCurve* newCurve2 = new QCPCurve(customPlot->yAxis, customPlot->xAxis);
    newCurve2->setData(x2, y2);

    // give the axes some labels:
    customPlot->xAxis->setLabel("x");
    customPlot->yAxis->setLabel("y");
    // set axes ranges, so we see all data:
    customPlot->replot();
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QCustomPlot* plot = new QCustomPlot;
    setup(plot);
    plot->show();

    return app.exec();
}
*/




/*
#include "model/BowModel.hpp"

int main()
{
    InputData2 input;
    input.load("../examples/layers.bow");

    OutputData output = BowModel::simulate(input, true, true);

    return 0;
}
*/

/*
#include "gui/SeriesEditor.hpp"

int main(int argc, char *argv[])
{
    QCoreApplication::setApplicationName("Bow Design Simulator");
    QCoreApplication::setApplicationVersion("0.0.0");
    QApplication app(argc, argv);

    QMainWindow window;
    window.show();

    DataSeries series;
    auto edit = new SeriesEditor(&window, series, [](const DataSeries& in)
    {
        //try
        //{
        //    CubicSpline spline(in);
        //    return spline.sample(100);
        //}
        //catch(std::runtime_error e)
        //{
        //    return DataSeries();
        //}

        return DataSeries();
    });

    edit->setInputLabels("x data", "y data");
    edit->setOutputLabels("x axis", "y axis");

    if(edit->exec() == QDialog::Accepted)
    {
        // ...
    }


    return app.exec();
}
*/


/*
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCore/QDateTime>
#include <QtCharts/QDateTimeAxis>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QDebug>
#include <QtCharts/QValueAxis>

QT_CHARTS_USE_NAMESPACE

class LinePlot: public  QChart
{
public:
    LineChart(const QString& title, const QString& x_label, const QString& y_label)
    {
        QValueAxis *axisX = new QValueAxis;
        axisX->setLabelFormat("%i");
        axisX->setTitleText(x_label);

        QValueAxis *axisY = new QValueAxis;
        axisY->setLabelFormat("%i");
        axisY->setTitleText(y_label);

        this->setTitle(title);
        this->addAxis(axisX, Qt::AlignBottom);
        this->addAxis(axisY, Qt::AlignLeft);
        this->legend()->hide();     // Todo: Make depenent on number of series
    }

    void addSeries(const DataSeries& data)
    {
        QLineSeries *series = new QLineSeries();
        for(size_t i = 0; i < data.size(); ++i)
        {
            series->append(data.arg(i), data.val(i));
        }

        QChart::addSeries(series);
        //series->attachAxis(this->axisX());
        //series->attachAxis(this->axisY());
        this->createDefaultAxes();  // Todo: Is there a better solution?
    }

};
*/
/*
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    LineChart* chart = new LineChart("Function plot", "x-axis", "y-axis");
    DataSeries series1({{0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0}}, {{0.0, 1.0, 4.0, 9.0, 16.0, 25.0, 36.0}});
    chart->addSeries(series1);

    DataSeries series2({{0.0, 2.0, 4.0, 6.0, 8.0, 10.0, 12.0}}, {{0.0, 1.0, 4.0, 9.0, 16.0, 25.0, 36.0}});
    chart->addSeries(series2);

    QChartView* chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setRubberBand(QChartView::RectangleRubberBand);

    QMainWindow window;
    window.setCentralWidget(chartView);
    window.resize(820, 600);
    window.show();

    return app.exec();
}
*/
