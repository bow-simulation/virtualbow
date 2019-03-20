#pragma once
#include "gui/PlotWidget.hpp"
#include "numerics/CubicSpline.hpp"

class SplineView: public PlotWidget
{
public:
    SplineView(const QString& x_label, const QString& y_label);
    qreal limb_length = {1.0};
    void setData(Series input);
};
