#pragma once
#include "gui/PlotWidget.hpp"
#include "numerics/ArcCurve.hpp"

class ProfileView: public PlotWidget
{
public:
    ProfileView();
    void setData(Series profile);

private:
    QCPCurve* curve0;
    QCPCurve* curve1;
};
