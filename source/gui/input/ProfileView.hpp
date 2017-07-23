#pragma once
#include "gui/PlotWidget.hpp"
#include "model/Document.hpp"

class InputData;

class ProfileView: public PlotWidget
{
public:
    ProfileView(InputData& data);
    void setSelection(const std::vector<int>& indices);

private:
    InputData& data;
    std::vector<Connection> connections;
    QCPCurve* curve0;
    QCPCurve* curve1;

    void update();
};
