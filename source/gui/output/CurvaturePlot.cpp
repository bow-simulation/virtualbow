#include "CurvaturePlot.hpp"
#include "gui/input/views/LayerColors.hpp"

CurvaturePlot::CurvaturePlot(const LimbProperties& limb, const BowStates& states)
    : limb(limb),
      states(states)
{
    this->xAxis->setLabel("Arc length [m]");
    this->yAxis->setLabel("Curvature [1/m]");

    this->addGraph();
    this->graph(0)->setName("Curvature");

    setAxesRanges();
}

void CurvaturePlot::setStateIndex(int index)
{
    this->graph(0)->setData(limb.length, states.kappa[index]);
    this->replot();
}

void CurvaturePlot::setAxesRanges()
{
    QCPRange x_range(limb.length.minCoeff(), limb.length.maxCoeff());
    QCPRange y_range(0.0, 0.0);

    for(size_t i = 0; i < states.kappa.size(); ++i)
    {
        for(size_t j = 1; j < states.kappa[i].size(); j++)
        {
            y_range.expand(states.kappa[i][j]);
        }
    }

    this->setAxesLimits(x_range, y_range);
}
