#pragma once
#include "bow/input/Profile.hpp"
#include "gui/PlotWidget.hpp"
#include "numerics/ArcCurve.hpp"

class ProfileView: public PlotWidget
{
public:
    ProfileView();
    void setData(Profile profile);

private:
    QCPCurve* curve0;
    QCPCurve* curve1;
};
