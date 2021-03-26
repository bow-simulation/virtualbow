#pragma once
#include "gui/PlotWidget.hpp"
#include "gui/units/UnitGroup.hpp"
#include "solver/numerics/Eigen.hpp"
#include "solver/numerics/Series.hpp"

class ProfileView: public PlotWidget {
public:
    ProfileView(const UnitGroup& unit);
    void setData(const MatrixXd& profile);
    void setSelection(const QVector<int>& selection);
private:
    const UnitGroup& unit;

    MatrixXd data;
    QVector<int> selection;

    QVector<QCPCurve*> curves;
    QVector<QCPCurve*> nodes;
    QCPItemText* label;

    void updatePlot();
    void resetPlot();
    void updateHighlights();
};
