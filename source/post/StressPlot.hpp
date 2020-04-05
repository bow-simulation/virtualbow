#pragma once
#include "gui/PlotWidget.hpp"
#include "solver/model//output/OutputData.hpp"
#include "solver/model//input/InputData.hpp"

class StressPlot: public PlotWidget
{
public:
    StressPlot(const LimbProperties& limb, const BowStates& states);
    void setStateIndex(int index);

private:
    const LimbProperties& limb;
    const BowStates& states;

    void setAxesRanges();
};
