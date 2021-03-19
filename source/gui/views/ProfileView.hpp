#pragma once
#include "gui/PlotWidget.hpp"
#include "gui/units/UnitGroup.hpp"
#include "solver/numerics/Eigen.hpp"
#include "solver/numerics/Series.hpp"

class ProfileView: public PlotWidget
{
public:
    ProfileView(const UnitGroup& xy_unit);
    void setData(const MatrixXd& profile);
    void setSelection(const QVector<int>& indices);

private:
    const UnitGroup& unit;

    MatrixXd input = MatrixXd::Zero(2, 2);
    QVector<int> selection;

    QCPCurve* curve0;
    QCPCurve* curve1;
    QCPCurve* curve2;

    void updatePlot();
};
