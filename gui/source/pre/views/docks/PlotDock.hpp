#pragma once
#include <QDockWidget>
#include <QPersistentModelIndex>
#include <QMap>
#include <functional>

class QLabel;
class MainModel;

class PlotDock: public QDockWidget {
public:
    PlotDock(MainModel* viewModel);

    void showPlaceholder();
    void showPlot(QPersistentModelIndex index, const std::function<QWidget*()>& create);

    /*
    template<typename F>
    void showPlot(QPersistentModelIndex index, const F& f) {
        if(!plots.contains(index)) {
            qInfo() << "Construct " << index;
            plots.insert(index, f());
        }

        setWidget(plots[index]);
    }
    */

private:
    QLabel* placeholder;
    QMap<QPersistentModelIndex, QWidget*> plots;
};
