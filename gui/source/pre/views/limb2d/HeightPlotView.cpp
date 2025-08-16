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
        //setNodesVisible(checked);
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

    xAxis->setLabel("Length " + Quantities::ratio.getUnit().getLabel());
    yAxis->setLabel("Height " + Quantities::length.getUnit().getLabel());

    graphLine->data()->clear();
    graphPoints->data()->clear();
    graphSelected->data()->clear();

    int iLayer = index.row();    // Layer index comes from row of the model index

    if(model->hasGeometry()) {
        for(size_t i = 0; i < model->getGeometry().ratio.size(); ++i) {
            graphLine->addData(
                Quantities::ratio.getUnit().fromBase(model->getGeometry().ratio[i]),
                Quantities::length.getUnit().fromBase(model->getGeometry().heights[i][iLayer])
            );
        }
    }

    if(model->hasBow()) {
        for(auto& point: model->getBow().section.layers[iLayer].height) {
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
