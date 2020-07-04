#pragma once
#include "gui/PlotWidget.hpp"
#include "solver/numerics/Eigen.hpp"
#include "solver/numerics/Series.hpp"

class ProfileView: public PlotWidget
{
public:
    ProfileView();
    void setData(const MatrixXd& profile);
    void setSelection(const QVector<int>& indices);
private:
    MatrixXd input = MatrixXd::Zero(0, 2);
    QVector<int> selection;

    QCPCurve* curve0;
    QCPCurve* curve1;
    QCPCurve* curve2;

    void updatePlot();
};
