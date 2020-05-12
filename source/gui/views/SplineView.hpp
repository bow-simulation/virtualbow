#pragma once
#include "gui/PlotWidget.hpp"
#include "solver/numerics/CubicSpline.hpp"
#include "solver/numerics/Series.hpp"

class SplineView: public PlotWidget
{
public:
    SplineView(const QString& x_label, const QString& y_label);
    void setData(Series data);
    void setSelection(const QVector<int>& indices);

private:
    Series input;
    QVector<int> selection;

    void updatePlot();
};
