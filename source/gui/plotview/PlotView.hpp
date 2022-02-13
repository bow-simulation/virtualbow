#pragma once
#include <QDockWidget>

class QStackedWidget;
class QLabel;

class PlotView: public QDockWidget {
public:
    PlotView();
    void showPlot(QWidget* plot);

private:
    QStackedWidget* stack;
    QLabel* placeholder;
};
