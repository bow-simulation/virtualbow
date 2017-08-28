#pragma once
#include "gui/PlotWidget.hpp"
#include "model/OutputData.hpp"

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
