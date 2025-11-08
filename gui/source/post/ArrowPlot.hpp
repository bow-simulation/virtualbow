#pragma once
#include "solver/BowResult.hpp"
#include <QWidget>

class QCPGraph;
class QCPRange;
class PlotWidget;

class ArrowPlot: public QWidget {
public:
    ArrowPlot(const Common& common, const States& states);

private:
    const Common& common;
    const States& states;

    PlotWidget* posPlot;
    PlotWidget* velPlot;
    PlotWidget* accPlot;

    QCPGraph* posGraph;
    QCPGraph* velGraph;
    QCPGraph* accGraph;

    void updateRanges(const QCPRange& range);
    void updatePlots();
};
