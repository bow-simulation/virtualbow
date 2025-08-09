#pragma once
#include "pre/widgets/PlotWidget.hpp"
#include "pre/models/units/Quantity.hpp"
#include "solver/BowResult.hpp"

class StressPlot: public PlotWidget {
public:
    StressPlot(const Common& common, const States& states);
    void setStateIndex(int i);

private:
    const Common& common;
    const States& states;
    int index;

    const Quantity& quantity_length;
    const Quantity& quantity_stress;

    void updatePlot();
    void updateStresses();
    void updateAxes();
};
