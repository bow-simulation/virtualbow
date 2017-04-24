#include "SplineView.hpp"
#include "model/InputData.hpp"
#include "numerics/CubicSpline.hpp"

SplineView::SplineView(const QString& x_label, const QString& y_label, DocItem<Series>& doc_item)
    : PlotWidget({750, 250})    // Todo: Magic number
{
    this->xAxis->setLabel(x_label);
    this->yAxis->setLabel(y_label);

    // Line
    this->addGraph();
    this->graph()->setPen({Qt::blue, 2});

    // Control points
    this->addGraph();
    this->graph()->setScatterStyle({QCPScatterStyle::ssSquare, Qt::red, 6});
    this->graph()->setLineStyle(QCPGraph::lsNone);

    connection = doc_item.connect([this](const Series& input)
    {
        try
        {
                    qInfo() << "Here";
            Series output = CubicSpline::sample(input, 150);    // Todo: Magic number
            this->graph(0)->setData(output.args(), output.vals());
            this->graph(1)->setData(input.args(), input.vals());
        }
        catch(std::runtime_error& e)
        {
            this->graph(0)->data()->clear();
            this->graph(1)->data()->clear();
        }

        this->rescaleAxes(false, true);
        this->replot();
    });
}
