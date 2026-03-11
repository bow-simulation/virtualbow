#pragma once
#include "pre/widgets/PlotWidget.hpp"

class MainModel;
class QCPGraph;

class WidthPlotView: public PlotWidget {
public:
    WidthPlotView(MainModel* model);

private:
    MainModel* model;
    QCPGraph* graphLine;
    QCPGraph* graphPoints;
    QCPGraph* graphSelected;

    void updatePlot();
    void setNodesVisible(bool visible);
};
