#pragma once
#include "pre/widgets/PlotWidget.hpp"

class MainModel;
class QCPGraph;

class HeightPlotView: public PlotWidget {
public:
    HeightPlotView(MainModel* model, QPersistentModelIndex index);
    void updatePlot();

private:
    MainModel* model;
    QPersistentModelIndex index;

    QCPGraph* graphLine;
    QCPGraph* graphPoints;
    QCPGraph* graphSelected;
};
