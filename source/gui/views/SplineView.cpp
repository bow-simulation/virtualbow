#include "SplineView.hpp"
#include "solver/numerics/Linspace.hpp"

SplineView::SplineView(const QString& x_label, const QString& y_label)
{
    qInfo() << "CONSTRUCT SPLINE_VIEW";

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

void SplineView::setData(const MatrixXd& data)
{
    qInfo() << "SET DATA";

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
    qInfo() << "UPDATE PLOT";

    this->graph(0)->data()->clear();
    this->graph(1)->data()->clear();
    this->graph(2)->data()->clear();

    // Line
    try
    {
        CubicSpline spline = CubicSpline(input);
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
    for(int i = 0; i < input.rows(); ++i)
    {
        if(selection.contains(i))
            this->graph(2)->addData(input(i, 0), input(i, 1));
        else
            this->graph(1)->addData(input(i, 0), input(i, 1));
    }

    this->rescaleAxes(false, true);
    this->replot();
}
