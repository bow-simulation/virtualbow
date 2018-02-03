#include "StressPlot.hpp"

StressPlot::StressPlot(const SetupData& setup, const BowStates& states)
    : setup(setup),
      states(states)
{
    this->xAxis->setLabel("Arc length [m]");
    this->yAxis->setLabel("Stress [N/m²]");
    this->setupTopLegend();

    this->addGraph();
    this->graph(0)->setPen({Qt::blue});
    this->graph(0)->setName("Back");

    this->addGraph();
    this->graph(1)->setPen({Qt::red});
    this->graph(1)->setName("Belly");

    setAxesRanges();
}

void StressPlot::setStateIndex(int index)
{
    const LayerProperties& layer = setup.limb.layers[0];
    this->graph(0)->setData(layer.s, layer.sigma_back(states.epsilon[index], states.kappa[index]));
    this->graph(1)->setData(layer.s, layer.sigma_belly(states.epsilon[index], states.kappa[index]));
    this->replot();
}

void StressPlot::setAxesRanges()
{
    QCPRange x_range(setup.limb.s.minCoeff(), setup.limb.s.maxCoeff());
    QCPRange y_range(0.0, 0.0);

    const LayerProperties& layer = setup.limb.layers[0];
    for(size_t i = 0; i < states.time.size(); ++i)
    {
        y_range.expand(layer.sigma_back(states.epsilon[i], states.kappa[i]).maxCoeff());
        y_range.expand(layer.sigma_belly(states.epsilon[i], states.kappa[i]).minCoeff());
    }

    this->setAxesLimits(x_range, y_range);
}
