#include "WidthPlotView.hpp"
#include "pre/models/MainModel.hpp"
#include "pre/models/units/UnitSystem.hpp"

WidthPlotView::WidthPlotView(MainModel* model):
    model(model)
{
    // Line
    graphLine = addGraph();
    graphLine->setName("Line");
    graphLine->setPen({Qt::blue, 2});

    // Control points
    graphPoints = addGraph();
    graphPoints->setName("Points");
    graphPoints->setScatterStyle({QCPScatterStyle::ssSquare, Qt::blue, 8});
    graphPoints->setLineStyle(QCPGraph::lsNone);

    // Selected points
    graphSelected = addGraph();
    graphSelected->setName("Selected");
    graphSelected->setScatterStyle({QCPScatterStyle::ssSquare, Qt::red, Qt::red, 8});
    graphSelected->setLineStyle(QCPGraph::lsNone);

    auto action_show_nodes = new QAction("Show nodes", this);
    action_show_nodes->setCheckable(true);
    action_show_nodes->setChecked(true);
    QObject::connect(action_show_nodes, &QAction::triggered, [&](bool checked) {
        //setNodesVisible(checked);
        replot();
    });

    QAction* before = contextMenu()->actions().isEmpty() ? nullptr : contextMenu()->actions().front();
    contextMenu()->insertAction(before, action_show_nodes);
    contextMenu()->insertSeparator(before);

    // Update on unit and geometry changes
    QObject::connect(&Quantities::ratio, &Quantity::unitChanged, this, &WidthPlotView::updatePlot);
    QObject::connect(&Quantities::length, &Quantity::unitChanged, this, &WidthPlotView::updatePlot);
    QObject::connect(model, &MainModel::geometryChanged, this, &WidthPlotView::updatePlot);

    // Initial update
    updatePlot();
}

void WidthPlotView::updatePlot() {
    xAxis->setLabel("Length " + Quantities::ratio.getUnit().getSuffix());
    yAxis->setLabel("Width " + Quantities::length.getUnit().getSuffix());

    graphLine->data()->clear();
    graphPoints->data()->clear();
    graphSelected->data()->clear();

    if(model->hasGeometry()) {
        graphLine->addData(
            Quantities::ratio.getUnit().fromBase(model->getGeometry().ratio),
            Quantities::length.getUnit().fromBase(model->getGeometry().width)
        );
    }

    if(model->hasBow()) {
        for(auto& point: model->getBow().section.width) {
            graphPoints->addData(
                Quantities::ratio.getUnit().fromBase(point[0]),
                Quantities::length.getUnit().fromBase(point[1])
            );
        }
    }

    /*
    // Control points
    for(int i = 0; i < input.size(); ++i) {
        if(selection.contains(i)) {
            this->graph(2)->addData(
                x_quantity.getUnit().fromBase(input[i][0]),
                y_quantity.getUnit().fromBase(input[i][1])
            );
        }
        else {
            this->graph(1)->addData(
                x_quantity.getUnit().fromBase(input[i][0]),
                y_quantity.getUnit().fromBase(input[i][1])
            );
        }
    }
    */

    this->rescaleAxes(true, true);
    this->replot();
}
