#pragma once
#include <QDockWidget>

class DataViewModel;

class PlotView: public QDockWidget
{
public:
    PlotView(DataViewModel* model);
};
