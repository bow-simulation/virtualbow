#pragma once
#include <QDockWidget>

class QStackedWidget;
class QLabel;

class PlotDock: public QDockWidget {
public:
    PlotDock();
    void showPlot(QWidget* plot);

private:
    QLabel* placeholder;
};
