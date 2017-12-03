#include "SplineView.hpp"
#include "bow/input/InputData.hpp"
#include "numerics/CubicSpline.hpp"

SplineView::SplineView(const QString& x_label, const QString& y_label, DocumentItem<Series>& doc_item)
    : doc_item(doc_item)
{
    this->xAxis->setLabel(x_label);
    this->yAxis->setLabel(y_label);

    // Line
    this->addGraph();

    // Control points
    this->addGraph();
    this->graph()->setScatterStyle({QCPScatterStyle::ssSquare, Qt::blue, 8});
    this->graph()->setLineStyle(QCPGraph::lsNone);

    // Selected points
    this->addGraph();
    this->graph()->setScatterStyle({QCPScatterStyle::ssSquare, Qt::red, 8});
    this->graph()->setLineStyle(QCPGraph::lsNone);

    QObject::connect(&doc_item, &DocumentNode::value_changed, this, &SplineView::update_value);
    update_value();
}

void SplineView::setSelection(const std::vector<int>& indices)
{
    selection = indices;
    update_plot();
}

void SplineView::update_value()
{
    try
    {
        input = doc_item;
        output = CubicSpline::sample(input, 150);    // Magic number
    }
    catch(std::runtime_error& e)
    {
        output = Series();
    }

    update_plot();
}

void SplineView::update_plot()
{
    std::vector<double> x_selected;
    std::vector<double> y_selected;
    for(int i: selection)
    {
        x_selected.push_back(input.arg(i));
        y_selected.push_back(input.val(i));
    }

    this->graph(0)->setData(output.args(), output.vals());
    this->graph(1)->setData(input.args(), input.vals());
    this->graph(2)->setData(x_selected, y_selected);

    this->rescaleAxes(false, true);
    this->replot();
}
