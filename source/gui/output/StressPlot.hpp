#pragma once
#include "gui/PlotWidget.hpp"
#include "bow/output/OutputData.hpp"
#include "bow/input/InputData.hpp"

class StressPlot: public PlotWidget
{
public:
    StressPlot(const InputData& input, const LimbProperties& limb, const BowStates& states);
    void setStateIndex(int index);

private:
    const LimbProperties& limb;
    const InputData& input;
    const BowStates& states;

    void setAxesRanges();
};
