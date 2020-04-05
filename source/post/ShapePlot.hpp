#pragma once
#include "gui/PlotWidget.hpp"
#include "solver/model//output/OutputData.hpp"

class ShapePlot: public PlotWidget
{
public:
    ShapePlot(const LimbProperties& limb, const BowStates& states, bool intermediate_steps);
    void setStateIndex(int i);

private:
    const LimbProperties& limb;
    const BowStates& states;

    QCPCurve* limb_right;
    QCPCurve* limb_left;
    QCPCurve* string_right;
    QCPCurve* string_left;
    QCPCurve* arrow;    // Todo: Replace with other QCustomPlot object?

    void plotIntermediateStates();
    void plotLimbOutline(QCPCurve* left, QCPCurve* right, const VectorXd& x, const VectorXd& y, const VectorXd& phi);
    void setAxesRanges();
};
