#include "gui/MainWindow.hpp"
#include "gui/SeriesEditor.hpp"
#include "numerics/CubicSpline.hpp"
#include <QtWidgets>

#include "gui/Document.hpp"

int main(int argc, char *argv[])
{
    QCoreApplication::setApplicationName("Bow Designer");
    QCoreApplication::setApplicationVersion("0.0");
    QApplication app(argc, argv);

    MainWindow window;
    window.show();

    return app.exec();
}

/*
#include "model/BowModel.hpp"

int main()
{
    InputData input;
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
        try
        {
            CubicSpline spline(in);
            return spline.sample(100);
        }
        catch(std::runtime_error e)
        {
            return DataSeries();
        }

        //return DataSeries();
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

void plot(const std::vector<double>& x, const std::vector<double>& y)
{
    // Todo: Is this shit really neccessary?
    int argc = 0; char** argv = nullptr;
    QApplication app(argc, argv);

    QLineSeries *series = new QLineSeries();
    series->setName("Parabola");
    for(size_t i = 0; i < x.size(); ++i)
    {
        series->append(x[i], y[i]);
    }

    QChart *chart = new QChart();
    chart->addSeries(series);
    //chart->legend()->hide();
    chart->setTitle("Sunspots count (by Space Weather Prediction Center)");

    QValueAxis *axisX = new QValueAxis;
    axisX->setLabelFormat("%i");
    axisX->setTitleText("x-Axis");
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis;
    axisY->setLabelFormat("%i");
    axisY->setTitleText("y-Axis");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setRubberBand(QChartView::RectangleRubberBand);

    QMainWindow window;
    window.setCentralWidget(chartView);
    window.resize(820, 600);
    window.show();

    app.exec();
}



int main(int argc, char *argv[])
{
    std::vector<double> x = {{0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0}};
    std::vector<double> y = {{0.0, 1.0, 4.0, 9.0, 16.0, 25.0, 36.0}};

    plot(x, y);

    return 0;
}
*/
