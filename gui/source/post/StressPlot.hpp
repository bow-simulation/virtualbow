#pragma once
#include "pre/widgets/PlotWidget.hpp"
#include "pre/viewmodel/units/Quantity.hpp"
#include "solver/model/output2/OutputData2.hpp"

class StressPlot: public PlotWidget {
public:
    StressPlot(const LimbSetup& limb, const StateVec& states);
    void setStateIndex(int i);

private:
    const LimbSetup& limb;
    const StateVec& states;
    int index;

    const Quantity& quantity_length;
    const Quantity& quantity_stress;

    void updatePlot();
    void updateStresses();
    void updateAxes();
};
