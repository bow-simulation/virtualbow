#pragma once
#include "gui/Plot.hpp"
#include "model/OutputData.hpp"

class ShapePlot: public Plot
{
public:
    ShapePlot(const BowSetup& setup, const BowStates& states, bool show_steps);
    void setStateIndex(int index);

private:
    const BowSetup& setup;
    const BowStates& states;

    size_t limb_l;
    size_t limb_r;
    size_t string_l;
    size_t string_r;
    size_t arrow;

    void plotIntermediateSteps();
    void setContentRanges();
};
