#pragma once
#include "gui/PlotWidget.hpp"
#include "bow/output/OutputData.hpp"

class StressPlot: public PlotWidget
{
public:
    StressPlot(const SetupData& setup, const BowStates& states);
    void setStateIndex(int index);

private:
    const SetupData& setup;
    const BowStates& states;

    void setAxesRanges();
};
