#pragma once
#include "gui/PlotWidget.hpp"
#include "solver/model/output/OutputData.hpp"
#include "solver/model/input/InputData.hpp"

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
