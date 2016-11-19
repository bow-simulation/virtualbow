#pragma once
#include "../../model/OutputData.hpp"
#include "../Plot.hpp"
#include <algorithm>

class EnergyPlot: public Plot
{
public:
    EnergyPlot(const BowStates& states, const std::vector<double>& parameter, const QString& x_label)
        : Plot(x_label, "Energy"),
          states(states),
          parameter(parameter),
          energy_sum(parameter.size(), 0.0)
    {
        addEnergy(states.e_pot_limbs, QColor(50, 50, 255, 150), "Limbs (Pot.)");
        addEnergy(states.e_kin_limbs, QColor(0, 0, 255, 150), "Limbs (Kin.)");
        addEnergy(states.e_pot_string, QColor(255, 50, 255, 150), "String (Pot.)");
        addEnergy(states.e_kin_string, QColor(255, 0, 255, 150), "String (Kin.)");
        addEnergy(states.e_kin_arrow, QColor(255, 0, 0, 150), "Arrow (Kin.)");

        this->fitContent(true, true);
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
    }

    /*
    void setShapeIndex(int index)
    {
        Series series_limb(states.pos_limb_x[index], states.pos_limb_y[index]);
        this->setData(limb_l, series_limb);
        this->setData(limb_r, series_limb.flip(false));

        Series series_string(states.pos_string_x[index], states.pos_string_y[index]);
        this->setData(string_l, series_string);
        this->setData(string_r, series_string.flip(false));

        Series series_arrow({0.0}, {states.pos_arrow[index]});
        this->setData(arrow, series_arrow);

        this->replot();
    }
    */

private:
    const BowStates& states;
    const std::vector<double>& parameter;
    std::vector<double> energy_sum;

};
