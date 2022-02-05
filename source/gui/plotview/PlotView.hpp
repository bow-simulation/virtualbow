#pragma once
#include <QDockWidget>

class QStackedWidget;

class PlotView: public QDockWidget {
public:
    PlotView();
    void showPlot(QWidget* plot);

private:
    QStackedWidget* stack;
    QWidget* placeholder;
};
