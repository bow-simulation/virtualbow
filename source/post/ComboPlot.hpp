#pragma once
#include "gui/PlotWidget.hpp"
#include "solver/model/output/OutputData.hpp"

class ComboPlot: public QWidget
{
public:
    ComboPlot();

    void addData(const QString& name, const std::vector<double>& data);
    void setCombination(int index_x, int index_y);

private:
    QComboBox* combo_x;
    QComboBox* combo_y;
    PlotWidget* plot;
    QCPCurve* curve;

    void updatePlot();
};
