#pragma once
#include "gui/PlotWidget.hpp"
#include "numerics/CubicSpline.hpp"

class SplineView: public PlotWidget
{
public:
    SplineView(const QString& x_label, const QString& y_label)
    {
        this->xAxis->setLabel(x_label);
        this->yAxis->setLabel(y_label);

        // Line
        this->addGraph();

        // Control points
        this->addGraph();
        this->graph()->setScatterStyle({QCPScatterStyle::ssSquare, Qt::red, 8});
        this->graph()->setLineStyle(QCPGraph::lsNone);
    }

    void setData(Series input)
    {
        Series output;

        try
        {
            output = CubicSpline::sample(input, 150);    // Magic number
        }
        catch(std::runtime_error& e)
        {
            output = Series();
        }

        this->graph(0)->setData(output.args(), output.vals());
        this->graph(1)->setData(input.args(), input.vals());

        this->rescaleAxes(false, true);
        this->replot();
    }
};
