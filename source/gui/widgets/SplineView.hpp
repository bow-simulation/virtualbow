#pragma once
#include "gui/widgets/PlotWidget.hpp"
#include "gui/units/UnitGroup.hpp"
#include "solver/numerics/Eigen.hpp"
#include "solver/numerics/CubicSpline.hpp"
#include "solver/numerics/Series.hpp"

class SplineView: public PlotWidget {
public:
    SplineView(const QString& x_label, const QString& y_label, const UnitGroup& x_unit, const UnitGroup& y_unit);
    void setData(const MatrixXd& data);
    void setSelection(const QVector<int>& indices);

private:
    QString x_label;
    QString y_label;
    const UnitGroup& x_unit;
    const UnitGroup& y_unit;

    MatrixXd input = MatrixXd::Zero(0, 2);
    QVector<int> selection;   

    void updatePlot();
    void setNodesVisible(bool visible);
};
