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
    this->graph(0)->setData(setup.limb.s, states.sigma_back[index]);
    this->graph(1)->setData(setup.limb.s, states.sigma_belly[index]);
    this->replot();
}

void StressPlot::setAxesRanges()
{
    QCPRange x_range(setup.limb.s.min(), setup.limb.s.max());
    QCPRange y_range;

    for(size_t i = 0; i < states.time.size(); ++i)
    {
        y_range.expand(states.sigma_back[i].max());
        y_range.expand(states.sigma_belly[i].min());
    }

    this->setAxesLimits(x_range, y_range);
}
