#include "HeightPlotView.hpp"
#include "pre/models/MainModel.hpp"
#include "pre/models/units/UnitSystem.hpp"

HeightPlotView::HeightPlotView(MainModel* model, QPersistentModelIndex index):
    model(model),
    index(index)
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
        setNodesVisible(checked);
        replot();
    });

    QAction* before = contextMenu()->actions().isEmpty() ? nullptr : contextMenu()->actions().front();
    contextMenu()->insertAction(before, action_show_nodes);
    contextMenu()->insertSeparator(before);

    // Update on unit and geometry changes
    QObject::connect(&Quantities::ratio, &Quantity::unitChanged, this, &HeightPlotView::updatePlot);
    QObject::connect(&Quantities::length, &Quantity::unitChanged, this, &HeightPlotView::updatePlot);
    QObject::connect(model, &MainModel::geometryChanged, this, &HeightPlotView::updatePlot);

    // Initial update
    updatePlot();
}

void HeightPlotView::updatePlot() {
    // Do nothing if the view is no longer associated with a valid model index
    if(!index.isValid()) {
        return;
    }

    // Y axis always shows the width in length units
    yAxis->setLabel("Height " + Quantities::length.getUnit().getLabel());

    graphLine->data()->clear();
    graphPoints->data()->clear();
    graphSelected->data()->clear();

    int iLayer = index.row();    // Layer index comes from row of the model index

    if(model->hasGeometry()) {
        // If the geometry is valid, the x axis shows the length in length units
        xAxis->setLabel("Length " + Quantities::length.getUnit().getLabel());

        // Line
        for(size_t i = 0; i < model->getGeometry().ratio.size(); ++i) {
            graphLine->addData(
                Quantities::length.getUnit().fromBase(model->getGeometry().length[i]),
                Quantities::length.getUnit().fromBase(model->getGeometry().heights[i][iLayer])
            );
        }

        // Points
        double length = model->getGeometry().length.back();     // Total length for scaling of the control points
        const Layer& layer = *std::next(model->getBow().section.layers.begin(), iLayer);
        for(auto& point: layer.height) {
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
        const Layer& layer = *std::next(model->getBow().section.layers.begin(), iLayer);
        for(auto& point: layer.height) {
            graphPoints->addData(
                Quantities::ratio.getUnit().fromBase(point[0]),
                Quantities::length.getUnit().fromBase(point[1])
            );
        }
    }

    rescaleAxes(true, true, 1.0, 1.05);
    replot();
}

void HeightPlotView::setNodesVisible(bool visible) {
    graphPoints->setVisible(visible);
    graphSelected->setVisible(visible);
}
