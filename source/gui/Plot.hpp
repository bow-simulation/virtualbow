#pragma once
#include "qcustomplot/qcustomplot.h"
#include "../model/Document.hpp"
#include "../numerics/Series.hpp"
#include "../numerics/CubicSpline.hpp"

class Plot: public QCustomPlot
{
public:
    void includeOrigin()
    {
        auto expand_axis = [this](QCPAxis* axis)
        {
            auto r = axis->range();
            r.expand(0.0);
            axis->setRange(r);
        };

        expand_axis(this->xAxis);
        expand_axis(this->yAxis);
        expand_axis(this->xAxis2);
        expand_axis(this->yAxis2);
    }
};
