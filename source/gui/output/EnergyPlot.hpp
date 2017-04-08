#pragma once
#include "gui/PlotWidget.hpp"
#include "model/OutputData.hpp"

class EnergyPlot: public QWidget
{
public:
    EnergyPlot(const BowStates& states, const std::vector<double>& parameter, const QString& x_label);

    //void addEnergy(const std::vector<double>& energy, const QColor& color, const QString& name);
    void setStateIndex(int index);

private:
    const BowStates& states;
    const std::vector<double>& parameter;
    int state_index;

    PlotWidget* plot;

    QCheckBox* checkbox_stacked;
    QCheckBox* checkbox_type;
    QCheckBox* checkbox_part;
    /*
    std::vector<double> energy_sum;
    std::vector<const std::vector<double>*> added_energies;
    std::vector<QString> added_names;
    */

    void updatePlot();
};
