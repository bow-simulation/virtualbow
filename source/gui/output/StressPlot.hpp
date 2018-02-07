#pragma once
#include "gui/PlotWidget.hpp"
#include "bow/output/OutputData.hpp"
#include "bow/input/InputData.hpp"

class StressPlot: public PlotWidget
{
public:
    StressPlot(const InputData& input, const SetupData& setup, const BowStates& states);
    void setStateIndex(int index);

private:
    const SetupData& setup;
    const InputData& input;
    const BowStates& states;

    void setAxesRanges();
};
