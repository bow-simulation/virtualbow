#pragma once
#include "gui/PlotWidget.hpp"
#include "model/Document.hpp"

class InputData;

class ProfileView: public PlotWidget
{
public:
    ProfileView(InputData& data);

private:
    InputData& data;
    std::vector<Connection> connections;
    QCPCurve* curve0;
    QCPCurve* curve1;

    void update();
};
