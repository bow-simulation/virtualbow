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
    // Y axis always shows the width in length units
    yAxis->setLabel("Width " + Quantities::length.getUnit().getLabel());

    graphLine->data()->clear();
    graphPoints->data()->clear();
    graphSelected->data()->clear();

    if(model->hasGeometry()) {
        // If the geometry is valid, the x axis shows the length in length units
        xAxis->setLabel("Length " + Quantities::length.getUnit().getLabel());

        // Line
        graphLine->addData(
            Quantities::length.getUnit().fromBase(model->getGeometry().length),
            Quantities::length.getUnit().fromBase(model->getGeometry().width)
        );

        // Points
        double length = model->getGeometry().length.back();     // Total length for scaling of the control points
        for(auto& point: model->getBow().section.width) {
            graphPoints->addData(
                Quantities::length.getUnit().fromBase(length*point[0]),
                Quantities::length.getUnit().fromBase(point[1])
            );
        }
    }
    else if(model->hasBow()) {
        // If the geometry is invalid, the x axis shows the length in relative units, so that the control points can still be shown
        xAxis->setLabel("Length " + Quantities::ratio.getUnit().getLabel());

        // Points
        for(auto& point: model->getBow().section.width) {
            graphPoints->addData(
                Quantities::ratio.getUnit().fromBase(point[0]),
                Quantities::length.getUnit().fromBase(point[1])
            );
        }
    }

    this->rescaleAxes(true, true, 1.0, 1.05);
    this->replot();
}
