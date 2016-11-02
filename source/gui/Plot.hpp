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
        auto range_x = this->xAxis->range();
        auto range_y = this->yAxis->range();
        range_x.expand(0.0);
        range_y.expand(0.0);

        this->xAxis->setRange(range_x);
        this->yAxis->setRange(range_y);
    }
};
