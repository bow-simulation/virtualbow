#include "SplineView.hpp"
#include "solver/numerics/Linspace.hpp"

SplineView::SplineView(const QString& x_label, const QString& y_label)
{
    this->xAxis->setLabel(x_label);
    this->yAxis->setLabel(y_label);

    // Control points
    this->addGraph();
    this->graph()->setScatterStyle({QCPScatterStyle::ssSquare, Qt::red, 8});
    this->graph()->setLineStyle(QCPGraph::lsNone);

    // Line
    this->addGraph();
    this->graph()->setPen({Qt::blue, 2});
}

void SplineView::setData(Series input)
{
    this->graph(0)->setData(input.args(), input.vals());
    this->graph(1)->data()->clear();

    try
    {
        CubicSpline spline = CubicSpline(input.args(), input.vals());
        for(double p: Linspace<double>(spline.arg_min(), spline.arg_max(), 100))    // Magic number
        {
            this->graph(1)->addData(p, spline(p));
        }
    }
    catch(std::invalid_argument& e)
    {

    }

    this->rescaleAxes(false, true);
    this->replot();
}
