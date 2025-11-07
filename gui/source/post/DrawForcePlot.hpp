#pragma once
#include "solver/BowResult.hpp"
#include <QWidget>

class QCPGraph;
class QCheckBox;
class PlotWidget;

class DrawForcePlot: public QWidget {
public:
    DrawForcePlot(const Common& common, const States& states);

private:
    const Common& common;
    const States& states;

    PlotWidget* plot;
    QCPGraph* forceGraph;
    QCPGraph* lineGraph;
    QCPGraph* stiffnessGraph;

    QCheckBox* cbLine;
    QCheckBox* cbStiffness;

    void updateVisibility();
    void updateGraphs();
};
