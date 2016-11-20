#pragma once
#include "../../model/OutputData.hpp"
#include "../Plot.hpp"
#include <algorithm>

class EnergyPlot: public Plot
{
public:
    EnergyPlot(const BowStates& states, const std::vector<double>& parameter, const QString& x_label)
        : Plot(x_label, "Energy"),
          parameter(parameter),
          energy_sum(parameter.size(), 0.0)
    {
        addEnergy(states.e_pot_limbs, QColor(0, 0, 255, 150), "Limbs (Pot.)");
        addEnergy(states.e_kin_limbs, QColor(40, 40, 255, 150), "Limbs (Kin.)");
        addEnergy(states.e_pot_string, QColor(128, 0, 128, 150), "String (Pot.)");
        addEnergy(states.e_kin_string, QColor(128, 40, 128, 150), "String (Kin.)");
        addEnergy(states.e_kin_arrow, QColor(255, 0, 0, 150), "Arrow (Kin.)");

        this->fitContent(false, true);
        this->showIndicatorX(true);
    }

    void addEnergy(const std::vector<double>& energy, const QColor& color, const QString& name)
    {
        auto reverse = [](const std::vector<double>& vec)
        {
            auto result = vec;
            std::reverse(result.begin(), result.end());
            return result;
        };

        auto append = [](std::vector<double> vec_a, std::vector<double> vec_b)
        {
            auto result = vec_a;
            result.insert(result.end(), vec_b.begin(), vec_b.end());
            return result;
        };

        auto energy_sum_new = energy_sum;
        for(size_t i = 0; i < energy.size(); ++i)
        {
            energy_sum_new[i] += energy[i];
        }

        // Don't display energies that are zero (kinetic energies in static simulation)
        if(energy_sum_new == energy_sum)
            return;

        auto loop_time = append(parameter, reverse(parameter));
        auto loop_energy = append(energy_sum, reverse(energy_sum_new));
        this->addSeries({loop_time, loop_energy}, Style::Brush(color), name);

        energy_sum = energy_sum_new;
        added_energies.push_back(&energy);
        added_names.push_back(name);
    }

    void setStateIndex(int index)
    {
        for(size_t i = 0; i < added_energies.size(); ++i)
        {
            QString value = QLocale::c().toString((*added_energies[i])[index], 'g', 5);
            this->setName(i, added_names[i] + "\n" + value + " J");
        }

        this->setIndicatorX(parameter[index]);
        this->replot();
    }

private:
    const std::vector<double>& parameter;

    std::vector<double> energy_sum;
    std::vector<const std::vector<double>*> added_energies;
    std::vector<QString> added_names;
};
