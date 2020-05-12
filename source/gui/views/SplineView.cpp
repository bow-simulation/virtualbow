#include "SplineView.hpp"
#include "solver/numerics/Linspace.hpp"

SplineView::SplineView(const QString& x_label, const QString& y_label)
{
    this->xAxis->setLabel(x_label);
    this->yAxis->setLabel(y_label);

    // Line
    this->addGraph();
    this->graph()->setPen({Qt::blue, 2});

    // Control points
    this->addGraph();
    this->graph()->setScatterStyle({QCPScatterStyle::ssSquare, Qt::blue, 8});
    this->graph()->setLineStyle(QCPGraph::lsNone);

    // Selected points
    this->addGraph();
    this->graph()->setScatterStyle({QCPScatterStyle::ssSquare, Qt::red, Qt::red, 8});
    this->graph()->setLineStyle(QCPGraph::lsNone);
}

void SplineView::setData(Series data)
{
    input = data;
    updatePlot();
}


void SplineView::setSelection(const QVector<int>& indices)
{
    selection = indices;
    updatePlot();
}

void SplineView::updatePlot()
{
    this->graph(0)->data()->clear();
    this->graph(1)->data()->clear();
    this->graph(2)->data()->clear();

    // Line
    try
    {
        CubicSpline spline = CubicSpline(input.args(), input.vals());
        for(double p: Linspace<double>(spline.arg_min(), spline.arg_max(), 100))    // Magic number
        {
            this->graph(0)->addData(p, spline(p));
        }
    }
    catch(std::invalid_argument& e)
    {
        this->graph(0)->data()->clear();
    }

    // Control points
    for(int i = 0; i < input.size(); ++i)
    {
        if(selection.contains(i))
            this->graph(2)->addData(input.arg(i), input.val(i));
        else
            this->graph(1)->addData(input.arg(i), input.val(i));
    }

    this->rescaleAxes(false, true);
    this->replot();
}
