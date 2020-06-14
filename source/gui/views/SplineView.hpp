#pragma once
#include "gui/PlotWidget.hpp"
#include "solver/numerics/Eigen.hpp"
#include "solver/numerics/CubicSpline.hpp"
#include "solver/numerics/Series.hpp"

class SplineView: public PlotWidget
{
public:
    SplineView(const QString& x_label, const QString& y_label);
    void setData(const MatrixXd& data);
    void setSelection(const QVector<int>& indices);

private:
    MatrixXd input = MatrixXd::Zero(0, 2);
    QVector<int> selection;

    void updatePlot();
};
