#include "StressPlot.hpp"

StressPlot::StressPlot(const BowSetup& setup, const BowStates& states)
    : Plot("Arc length [m]", "Stress [N/m²]"),
      setup(setup),
      states(states)
{
    this->addSeries({}, Style::Line(Qt::blue), "Back");
    this->addSeries({}, Style::Line(Qt::red), "Belly");
    setContentRanges();
}

void StressPlot::setStateIndex(int index)
{
    this->setData(0, {setup.limb.s, states.sigma_upper[index]});
    this->setData(1, {setup.limb.s, states.sigma_lower[index]});
    this->replot();
}

void StressPlot::setContentRanges()
{
    QCPRange rx(setup.limb.s.front(), setup.limb.s.back());
    QCPRange ry;

    auto expand = [&](const std::vector<double>& values_y)
    {
        for(double y: values_y)
        {
            ry.expand(y);
        }
    };

    for(size_t i = 0; i < states.time.size(); ++i)
    {
        expand(states.sigma_upper[i]);
        expand(states.sigma_lower[i]);
    }

    Plot::setContentRanges(rx, ry);
}
