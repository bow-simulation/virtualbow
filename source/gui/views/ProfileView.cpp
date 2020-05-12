#include "ProfileView.hpp"
#include "solver/model//ProfileCurve.hpp"
#include "solver/numerics/Linspace.hpp"

ProfileView::ProfileView()
{
    this->xAxis->setLabel("X [m]");
    this->yAxis->setLabel("Y [m]");
    this->setAspectPolicy(PlotWidget::SCALE_Y);

    // Line
    curve0 = new QCPCurve(this->xAxis, this->yAxis);
    curve0->setPen({Qt::blue, 2});
    curve0->setScatterSkip(0);    // Having to explicitly state this is retarded

    // Nodes
    curve1 = new QCPCurve(this->xAxis, this->yAxis);
    curve1->setScatterStyle({QCPScatterStyle::ssSquare, Qt::blue, 8});
    curve1->setLineStyle(QCPCurve::lsNone);
    curve1->setScatterSkip(0);    // Todo: Having to explicitly state this is retarded

    // Selected nodes
    curve2 = new QCPCurve(this->xAxis, this->yAxis);
    curve2->setScatterStyle({QCPScatterStyle::ssSquare, Qt::red, Qt::red, 8});
    curve2->setLineStyle(QCPCurve::lsNone);
    curve2->setScatterSkip(0);    // Todo: Having to explicitly state this is retarded
}

void ProfileView::setData(Series data)
{
    input = data;
    updatePlot();
}

void ProfileView::setSelection(const QVector<int>& indices)
{
    selection = indices;
    updatePlot();
}

void ProfileView::updatePlot()
{
    curve0->data()->clear();
    curve1->data()->clear();
    curve2->data()->clear();

    try
    {
        ProfileCurve profile(input.args(), input.vals(), 0.0, 0.0, 0.0);

        // Add interpolated points of the profile curve
        for(double s: Linspace<double>(profile.s_min(), profile.s_max(), 150))    // Magic number
        {
            Vector<3> point = profile(s);
            curve0->addData(point[0], point[1]);
        }

        // Add control points depending on selection status
        for(size_t i = 0; i < input.size(); ++i)
        {
            Vector<3> point = profile(input.arg(i));
            if(selection.contains(i))
                curve2->addData(point[0], point[1]);
            else
                curve1->addData(point[0], point[1]);
        }
    }
    catch(const std::invalid_argument&)
    {
        curve0->data()->clear();
        curve1->data()->clear();
        curve2->data()->clear();
    }

    this->rescaleAxes();
    this->replot();
}
