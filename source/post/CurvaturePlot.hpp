#pragma once
#include "gui/widgets/PlotWidget.hpp"
#include "gui/units/UnitSystem.hpp"
#include "solver/model/output/OutputData.hpp"
#include "solver/model/input/InputData.hpp"

class CurvaturePlot: public PlotWidget {
public:
    CurvaturePlot(const LimbProperties& limb, const BowStates& states);
    void setStateIndex(int i);

private:
    const LimbProperties& limb;
    const BowStates& states;
    int index;

    const UnitGroup& unit_length;
    const UnitGroup& unit_curvature;

    void updatePlot();
    void updateCurvature();
    void updateAxes();
};
