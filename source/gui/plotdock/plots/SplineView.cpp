#include "SplineView.hpp"
#include "solver/numerics/Linspace.hpp"

SplineView::SplineView(const QString& x_label, const QString& y_label, const Quantity& x_quantity, const Quantity& y_quantity)
    : x_label(x_label), y_label(y_label), x_quantity(x_quantity), y_quantity(y_quantity)
{
    // Line
    this->addGraph();
    this->graph()->setName("Line");
    this->graph()->setPen({Qt::blue, 2});

    // Control points
    this->addGraph();
    this->graph()->setName("Points");
    this->graph()->setScatterStyle({QCPScatterStyle::ssSquare, Qt::blue, 8});
    this->graph()->setLineStyle(QCPGraph::lsNone);

    // Selected points
    this->addGraph();
    this->graph()->setName("Selected");
    this->graph()->setScatterStyle({QCPScatterStyle::ssSquare, Qt::red, Qt::red, 8});
    this->graph()->setLineStyle(QCPGraph::lsNone);

    auto action_show_nodes = new QAction("Show nodes", this);
    action_show_nodes->setCheckable(true);
    action_show_nodes->setChecked(true);
    QObject::connect(action_show_nodes, &QAction::triggered, [&](bool checked) {
        setNodesVisible(checked);
        this->replot();
    });

    QAction* before = this->contextMenu()->actions().isEmpty() ? nullptr : this->contextMenu()->actions().front();
    this->contextMenu()->insertAction(before, action_show_nodes);
    this->contextMenu()->insertSeparator(before);

    // Update on unit changes
    QObject::connect(&x_quantity, &Quantity::unitChanged, this, &SplineView::updatePlot);
    QObject::connect(&y_quantity, &Quantity::unitChanged, this, &SplineView::updatePlot);
}

void SplineView::setData(const std::vector<Vector<2>>& data) {
    input = data;
    updatePlot();
}

void SplineView::setSelection(const QVector<int>& indices) {
    selection = indices;
    updatePlot();
}

void SplineView::updatePlot() {
    this->xAxis->setLabel(x_label + " " + x_quantity.getUnit().getLabel());
    this->yAxis->setLabel(y_label + " " + y_quantity.getUnit().getLabel());

    this->graph(0)->data()->clear();
    this->graph(1)->data()->clear();
    this->graph(2)->data()->clear();

    // Line
    try {
        CubicSpline spline = CubicSpline(input, true);
        for(double p: Linspace<double>(spline.arg_min(), spline.arg_max(), 500)) {    // Magic number
            this->graph(0)->addData(
                x_quantity.getUnit().fromBase(p),
                y_quantity.getUnit().fromBase(spline(p))
            );
        }
    }
    catch(std::exception& e) {
        // TODO: Show error message on plot
        this->graph(0)->data()->clear();
    }

    // Control points
    for(int i = 0; i < input.size(); ++i) {
        if(selection.contains(i)) {
            this->graph(2)->addData(
                x_quantity.getUnit().fromBase(input[i](0)),
                y_quantity.getUnit().fromBase(input[i](1))
            );
        }
        else {
            this->graph(1)->addData(
                x_quantity.getUnit().fromBase(input[i](0)),
                y_quantity.getUnit().fromBase(input[i](1))
            );
        }
    }

    this->rescaleAxes(false, true);
    this->replot();
}

void SplineView::setNodesVisible(bool visible) {
    this->graph(1)->setVisible(visible);
    this->graph(2)->setVisible(visible);
}
