#pragma once
#include "pre/widgets/PlotWidget.hpp"
#include "pre/viewmodel/units/Quantity.hpp"
#include "solver/model/output2/OutputData2.hpp"

class CurvaturePlot: public PlotWidget {
public:
    CurvaturePlot(const LimbSetup& limb, const StateVec& states);
    void setStateIndex(int i);

private:
    const LimbSetup& limb;
    const StateVec& states;
    int index;

    const Quantity& quantity_length;
    const Quantity& quantity_curvature;

    void updatePlot();
    void updateCurvature();
    void updateAxes();
};
