#include "gui/MainWindow.hpp"
#include "gui/SeriesEditor.hpp"
#include "numerics/SplineFunction.hpp"
#include <QtWidgets>

#include "gui/Document.hpp"


#include "model/BowModel.hpp"

int main()
{
    InputData input;
    input.load("../examples/layers.bow");

    OutputData output = BowModel::simulate(input, true, true);


    /*
    InputData data;
    data.load("../examples/layers.bow");

    DiscreteLimb limb(data);

    for(size_t i = 0; i < limb.s.size(); ++i)
    {
        qInfo() << limb.s[i] << ", " << limb.hc[i];
    }
    */

    return 0;
}


/*
int main(int argc, char *argv[])
{
    QCoreApplication::setApplicationName("Bow Design Simulator");
    QCoreApplication::setApplicationVersion("0.0.0");
    QApplication app(argc, argv);

    MainWindow window;
    window.show(); 

    return app.exec();
}
*/

/*
CurveEditor *edit = new CurveEditor(&window, [](const std::vector<double>& x_in,
                                                const std::vector<double>& y_in,
                                                std::vector<double>& x_out,
                                                std::vector<double>& y_out)
{
    SplineFunction::Parameters param{x_in, y_in};
    if(param.is_valid())
    {
        SplineFunction spline({x_in, y_in});
        spline.sample(x_out, y_out, 100);
    }
});

std::vector<double> x = {{0.0, 1.0, 2.0}};
std::vector<double> y = {{0.0, 1.0, 4.0}};
edit->setInputData(x, y);
edit->setInputLabels("x data", "y data");
edit->setOutputLabels("x axis", "y axis");

if(edit->exec() == QDialog::Accepted)
{
    std::vector<double> x, y;
    edit->getInputData(x, y);

    // ...
}
*/

/*
#include "numerics/StepFunction.hpp"

#include <Eigen/Core>
#include <boost/numeric/odeint.hpp>
#include <iostream>
#include <array>


int main()
{
    using namespace boost::numeric::odeint;
    typedef std::array<double, 3> State;

    StepFunction kappa({{0.4, 0.3, 0.2}}, {{0.1, 0.2, 1.5}});

    auto ode = [&](const State &z, State &dzds, const double s)
    {
        dzds[0] = std::cos(z[2]);
        dzds[1] = std::sin(z[2]);
        dzds[2] = kappa(s);
    };

    auto observer = [](const State& z , double s)
    {
        std::cout << z[0] << ", " << z[1] << "\n";
    };

    double mid_section_length = 0.5;
    double mid_section_angle = -0.1;

    State x0 = {{0.0, mid_section_length/2.0, mid_section_angle + M_PI_2}};

    typedef runge_kutta_dopri5<State, double> stepper_type;
    integrate_const(make_dense_output<stepper_type>(1e-8 , 1e-6),
                    ode, x0, kappa.arg_min(), kappa.arg_max() , 0.01, observer);

    return 0;
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

/*
#include <QtCharts>
#include <cereal/archives/json.hpp>

struct MyClass
{
    int x, y, z;

    template<class Archive>
    void serialize(Archive & archive)
    {std::vector<double> lengths, std::vector<double> values
        archive(x, y, z);
    }
};

#include <cereal/archives/xml.hpp>
#include <fstream>

int main()
{
    int someInt;
    double d;

    std::ofstream os("data.json");
    cereal::JSONOutputArchive archive(os);

    archive(CEREAL_NVP(someInt),
            CEREAL_NVP(d));
}
*/



/*
#include "numerics/StepFunction.hpp"
#include "numerics/SplineFunction.hpp"

#include <iostream>

int main()
{
    std::vector<double> l = {4.0, 5.0, 2.0, 5.0, 3.0};
    std::vector<double> v = {1.0, 2.0, 3.0, 1.0, -1.0};

    StepFunction fn(l, v);

    std::cout << fn(25);

    return 0;
}
*/
