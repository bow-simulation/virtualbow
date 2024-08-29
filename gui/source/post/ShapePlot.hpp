#pragma once
#include "pre/widgets/PlotWidget.hpp"
#include "pre/viewmodel/units/Quantity.hpp"
#include "solver/model/output/OutputData.hpp"

class ShapePlot: public PlotWidget {
public:
    ShapePlot(const Common& common, const States& states, int background_states);
    void setStateIndex(int i);

private:
    const Common& common;
    const States& states;
    const Quantity& quantity;

    int background_states;
    int index;

    QList<QCPCurve*> limb_right;
    QList<QCPCurve*> limb_left;
    QList<QCPCurve*> string_right;
    QList<QCPCurve*> string_left;
    QCPCurve* arrow;    // Todo: Replace with other QCustomPlot object?

    void updatePlot();

    void updateBackgroundStates();
    void updateCurrentState();
    void updateAxes();

    void plotLimbOutline(QCPCurve* left, QCPCurve* right, const std::vector<std::array<double, 3>>& position);
    void plotString(QCPCurve* left, QCPCurve* right, const std::vector<std::array<double, 2>>& position);
    void plotArrow(double position);
};
