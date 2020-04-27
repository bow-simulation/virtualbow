#pragma once
#include "gui/PlotWidget.hpp"
#include "solver/numerics/Series.hpp"

class ProfileView: public PlotWidget
{
public:
    ProfileView();
    void setData(Series profile);
    void setSelection(const QVector<int>& indices);
private:
    Series input;
    QVector<int> selection;

    QCPCurve* curve0;
    QCPCurve* curve1;
    QCPCurve* curve2;

    void updatePlot();
};
