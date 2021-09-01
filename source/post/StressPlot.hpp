#pragma once
#include "gui/PlotWidget.hpp"
#include "gui/units/UnitSystem.hpp"
#include "solver/model/output/OutputData.hpp"
#include "solver/model/input/InputData.hpp"

class StressPlot: public PlotWidget {
public:
    StressPlot(const LimbProperties& limb, const BowStates& states);
    void setStateIndex(int i);

private:
    const LimbProperties& limb;
    const BowStates& states;
    int index;

    const UnitGroup& unit_length;
    const UnitGroup& unit_stress;

    void updatePlot();
    void updateStresses();
    void updateAxes();
};
