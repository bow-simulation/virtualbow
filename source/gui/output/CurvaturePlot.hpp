#pragma once
#include "gui/PlotWidget.hpp"
#include "bow/output/OutputData.hpp"
#include "bow/input/InputData.hpp"

class CurvaturePlot: public PlotWidget
{
public:
    CurvaturePlot(const LimbProperties& limb, const BowStates& states);
    void setStateIndex(int index);

private:
    const LimbProperties& limb;
    const BowStates& states;

    void setAxesRanges();
};
