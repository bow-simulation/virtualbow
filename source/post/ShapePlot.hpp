#pragma once
#include "gui/PlotWidget.hpp"
#include "gui/units/UnitSystem.hpp"
#include "solver/model/output/OutputData.hpp"

class ShapePlot: public PlotWidget {
public:
    ShapePlot(const LimbProperties& limb, const BowStates& states, const UnitSystem& units, int background_states);
    void setStateIndex(int i);

private:
    const LimbProperties& limb;
    const BowStates& states;
    const UnitGroup& unit;

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

    void plotLimbOutline(QCPCurve* left, QCPCurve* right, const VectorXd& x, const VectorXd& y, const VectorXd& phi);
};
