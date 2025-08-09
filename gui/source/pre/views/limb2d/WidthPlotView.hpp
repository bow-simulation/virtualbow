#pragma once
#include "pre/widgets/PlotWidget.hpp"

class MainModel;
class QCPGraph;

class WidthPlotView: public PlotWidget {
public:
    WidthPlotView(MainModel* model);
    void updatePlot();

private:
    MainModel* model;
    QCPGraph* graphLine;
    QCPGraph* graphPoints;
    QCPGraph* graphSelected;
};
