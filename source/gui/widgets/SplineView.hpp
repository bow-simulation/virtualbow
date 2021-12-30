#pragma once
#include "gui/widgets/PlotWidget.hpp"
#include "gui/units/UnitGroup.hpp"
#include "solver/numerics/EigenTypes.hpp"
#include "solver/numerics/CubicSpline.hpp"

class SplineView: public PlotWidget {
public:
    SplineView(const QString& x_label, const QString& y_label, const UnitGroup& x_unit, const UnitGroup& y_unit);
    void setData(const std::vector<Vector<2>>& data);
    void setSelection(const QVector<int>& indices);

private:
    QString x_label;
    QString y_label;
    const UnitGroup& x_unit;
    const UnitGroup& y_unit;

    std::vector<Vector<2>> input;
    QVector<int> selection;   

    void updatePlot();
    void setNodesVisible(bool visible);
};
