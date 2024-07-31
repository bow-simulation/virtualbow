#include "CurvaturePlot.hpp"
#include "pre/limbview/LayerColors.hpp"

CurvaturePlot::CurvaturePlot(const LimbProperties& limb, const BowStates& states)
    : limb(limb),
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
    this->graph(0)->setData(
        quantity_length.getUnit().fromBase(limb.length),
        quantity_curvature.getUnit().fromBase(states.kappa[index])
    );
}


void CurvaturePlot::updateAxes() {
    this->xAxis->setLabel("Arc length " + quantity_length.getUnit().getLabel());
    this->yAxis->setLabel("Curvature " + quantity_curvature.getUnit().getLabel());

    QCPRange x_range(
        quantity_length.getUnit().fromBase(limb.length.minCoeff()),
        quantity_length.getUnit().fromBase(limb.length.maxCoeff())
    );
    QCPRange y_range(
        0.0,
        0.0
    );

    for(size_t i = 0; i < states.kappa.size(); ++i) {
        for(size_t j = 1; j < states.kappa[i].size(); j++) {
            y_range.expand(quantity_curvature.getUnit().fromBase(states.kappa[i][j]));
        }
    }

    this->setAxesLimits(x_range, y_range);
}
