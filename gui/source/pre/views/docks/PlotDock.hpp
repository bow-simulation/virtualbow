#pragma once
#include <QDockWidget>
#include <QPersistentModelIndex>
#include <QMap>
#include <functional>

class QLabel;
class MainModel;

class PlotDock: public QDockWidget {
public:
    PlotDock(MainModel* model);

    void showPlaceholder();
    void showPlot(QPersistentModelIndex index, const std::function<QWidget*()>& create);

private:
    QLabel* placeholder;
    QMap<QPersistentModelIndex, QWidget*> plots;
};
