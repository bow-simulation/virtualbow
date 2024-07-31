#pragma once
#include "pre/widgets/PlotWidget.hpp"
#include "pre/viewmodel/units/UnitSystem.hpp"
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

    const Quantity& quantity_length;
    const Quantity& quantity_curvature;

    void updatePlot();
    void updateCurvature();
    void updateAxes();
};
