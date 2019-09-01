#include "SplineView.hpp"

SplineView::SplineView(const QString& x_label, const QString& y_label)
{
    this->xAxis->setLabel(x_label);
    this->yAxis->setLabel(y_label);

    // Line
    this->addGraph();
    this->graph()->setPen({Qt::blue, 2});

    // Control points
    this->addGraph();
    this->graph()->setScatterStyle({QCPScatterStyle::ssSquare, Qt::red, 8});
    this->graph()->setLineStyle(QCPGraph::lsNone);
}

void SplineView::setData(Series input)
{
    Series output;

    try
    {
        output = CubicSpline(input).sample(150);    // Magic number
    }
    catch(std::invalid_argument& e)
    {
        output = Series();
    }

    this->graph(0)->setData(output.args(), output.vals());
    this->graph(1)->setData(input.args(), input.vals());

    this->rescaleAxes(false, true);
    this->replot();
}
