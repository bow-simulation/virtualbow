#include "CurvaturePlot.hpp"
#include "gui/limbview/LayerColors.hpp"

CurvaturePlot::CurvaturePlot(const LimbProperties& limb, const BowStates& states)
    : limb(limb),
      states(states),
      index(0),
      unit_length(UnitSystem::length),
      unit_curvature(UnitSystem::curvature)
{
    this->addGraph();
    this->graph(0)->setName("Curvature");
    this->graph(0)->setPen({Qt::blue, 2.0});

    QObject::connect(&unit_length, &UnitGroup::selectionChanged, this, &CurvaturePlot::updatePlot);
    QObject::connect(&unit_curvature, &UnitGroup::selectionChanged, this, &CurvaturePlot::updatePlot);
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
        unit_length.getSelectedUnit().fromBase(limb.length),
        unit_curvature.getSelectedUnit().fromBase(states.kappa[index])
    );
}


void CurvaturePlot::updateAxes() {
    this->xAxis->setLabel("Arc length " + unit_length.getSelectedUnit().getLabel());
    this->yAxis->setLabel("Curvature " + unit_curvature.getSelectedUnit().getLabel());

    QCPRange x_range(
        unit_length.getSelectedUnit().fromBase(limb.length.minCoeff()),
        unit_length.getSelectedUnit().fromBase(limb.length.maxCoeff())
    );
    QCPRange y_range(
        0.0,
        0.0
    );

    for(size_t i = 0; i < states.kappa.size(); ++i) {
        for(size_t j = 1; j < states.kappa[i].size(); j++) {
            y_range.expand(unit_curvature.getSelectedUnit().fromBase(states.kappa[i][j]));
        }
    }

    this->setAxesLimits(x_range, y_range);
}
