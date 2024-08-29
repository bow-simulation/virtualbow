#include "CurvaturePlot.hpp"
#include "pre/viewmodel/units/UnitSystem.hpp"

CurvaturePlot::CurvaturePlot(const Common& common, const States& states)
    : common(common),
      states(states),
      index(0),
      quantity_length(Quantities::length),
      quantity_curvature(Quantities::curvature)
{
    this->addGraph();
    this->graph(0)->setName("Curvature");
    this->graph(0)->setPen({Qt::blue, 2.0});

    QObject::connect(&quantity_length, &Quantity::unitChanged, this, &CurvaturePlot::updatePlot);
    QObject::connect(&quantity_curvature, &Quantity::unitChanged, this, &CurvaturePlot::updatePlot);
    updatePlot();
}

void CurvaturePlot::setStateIndex(int i) {
    index = i;
    updateCurvature();
    this->replot();
}

void CurvaturePlot::updatePlot() {
    updateCurvature();
    updateAxes();
    this->replot();
}

void CurvaturePlot::updateCurvature() {
    this->graph(0)->data()->clear();
    for(size_t i = 0; i < common.limb.length.size(); ++i) {
        this->graph(0)->addData(
            quantity_length.getUnit().fromBase(common.limb.length[i]),
            quantity_curvature.getUnit().fromBase(states.limb_strain[index][i][0])
        );
    }
}

void CurvaturePlot::updateAxes() {
    this->xAxis->setLabel("Arc length " + quantity_length.getUnit().getLabel());
    this->yAxis->setLabel("Curvature " + quantity_curvature.getUnit().getLabel());

    QCPRange x_range(
        quantity_length.getUnit().fromBase(common.limb.length.front()),
        quantity_length.getUnit().fromBase(common.limb.length.back())
    );

    QCPRange y_range(
        0.0,
        0.0
    );

    for(size_t i = 0; i < states.limb_strain.size(); ++i) {
        for(size_t j = 1; j < states.limb_strain[i].size(); j++) {
            y_range.expand(quantity_curvature.getUnit().fromBase(states.limb_strain[i][j][0]));
        }
    }

    this->setAxesLimits(x_range, y_range);
}
