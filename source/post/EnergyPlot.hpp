#pragma once
#include "gui/PlotWidget.hpp"
#include "solver/model/output/OutputData.hpp"

class EnergyPlot: public QWidget
{
public:
    EnergyPlot(const BowStates& states, const std::vector<double>& parameter, const QString& x_label);
    void setStateIndex(int index);

private:
    const BowStates& states;
    const std::vector<double>& parameter;

    PlotWidget* plot;

    QCheckBox* cb_stacked;
    QCheckBox* cb_part;
    QCheckBox* cb_type;

    void updatePlot();
};
