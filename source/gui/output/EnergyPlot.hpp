#pragma once
#include "gui/Plot.hpp"
#include "model/OutputData.hpp"

class EnergyPlot: public Plot
{
public:
    EnergyPlot(const BowStates& states, const std::vector<double>& parameter, const QString& x_label);

    void addEnergy(const std::vector<double>& energy, const QColor& color, const QString& name);
    void setStateIndex(int index);

private:
    const std::vector<double>& parameter;

    std::vector<double> energy_sum;
    std::vector<const std::vector<double>*> added_energies;
    std::vector<QString> added_names;
};
