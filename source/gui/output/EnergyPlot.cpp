#include "EnergyPlot.hpp"
#include "gui/HorizontalLine.hpp"
#include <algorithm>

EnergyPlot::EnergyPlot(const BowStates& states, const std::vector<double>& parameter, const QString& x_label)
    : states(states),
      parameter(parameter),
      state_index(0),
      plot(new PlotWidget()),
      checkbox_stacked(new QCheckBox("Stacked")),
      checkbox_type(new QCheckBox("Group by type")),
      checkbox_part(new QCheckBox("Group by component"))
    //energy_sum(parameter.size(), 0.0)
{
    plot->xAxis->setLabel(x_label);
    plot->yAxis->setLabel("Energy [J]");
    plot->setupTopLegend();

    auto vbox = new QVBoxLayout();
    this->setLayout(vbox);
    vbox->setContentsMargins({});
    vbox->setSpacing(0);
    vbox->addWidget(plot, 1);
    vbox->addSpacing(10);    // Magic number

    vbox->addWidget(new HorizontalLine());
    vbox->addSpacing(10);    // Magic number

    auto hbox = new QHBoxLayout();
    vbox->addLayout(hbox);
    vbox->addSpacing(10);    // Magic number

    hbox->addStretch();
    hbox->addWidget(checkbox_stacked);
    hbox->addSpacing(20);    // Magic number
    hbox->addWidget(checkbox_type);
    hbox->addSpacing(20);    // Magic number
    hbox->addWidget(checkbox_part);
    hbox->addStretch();

    QObject::connect(checkbox_stacked, &QCheckBox::toggled, this, &EnergyPlot::updatePlot);
    QObject::connect(checkbox_type, &QCheckBox::toggled, this, &EnergyPlot::updatePlot);
    QObject::connect(checkbox_part, &QCheckBox::toggled, this, &EnergyPlot::updatePlot);
    checkbox_stacked->setChecked(true);

    // Todo
    checkbox_type->setEnabled(false);
    checkbox_part->setEnabled(false);
}

void EnergyPlot::setStateIndex(int index)
{
    state_index = index;

    /*
    for(size_t i = 0; i < added_energies.size(); ++i)
    {
        QString value = QLocale::c().toString((*added_energies[i])[index], 'g', 5);
        plot->setName(i, added_names[i] + "\n" + value + " J");
    }

    plot->setIndicatorX(parameter[index]);
    plot->replot();
    */
}

void EnergyPlot::updatePlot()
{
    plot->clearPlottables();

    if(checkbox_stacked->isChecked())
    {
        std::vector<double> energy_lower;
        std::vector<double> energy_upper(parameter.size(), 0.0);

        auto add_energy = [&](const std::vector<double>& energy, const QString& name, const QColor& color)
        {
            // Test if energy is nonzero
            if(std::any_of(energy.begin(), energy.end(), [](double e) { return e > 0.0; }))
            {
                energy_lower = energy_upper;
                for(size_t i = 0; i < parameter.size(); ++i)
                {
                    energy_upper[i] += energy[i];
                }

                auto graph_lower = plot->graph();

                auto graph_upper = plot->addGraph();
                graph_upper->setData(parameter, energy_upper);
                graph_upper->setName(name);
                graph_upper->setBrush(color);
                //graph_upper->setPen(Qt::NoPen);
                graph_upper->setPen(color);

                if(graph_lower != nullptr)
                {
                    graph_upper->setChannelFillGraph(graph_lower);
                }
            }
        };

        // Todo: Make this a for-each loop with tuples and c++17 structured bindings and shit
        add_energy(states.e_pot_limbs, "Limbs (Pot)", QColor(0, 0, 255, 180));
        add_energy(states.e_kin_limbs, "Limbs (Kin)", QColor(40, 40, 255, 180));
        add_energy(states.e_pot_string, "String (Pot)", QColor(128, 0, 128, 180));
        add_energy(states.e_kin_string, "String (Kin)", QColor(128, 40, 128, 180));
        add_energy(states.e_kin_arrow, "Arrow (Kin)", QColor(255, 0, 0, 180));
    }
    else
    {
        auto add_energy = [&](const std::vector<double>& energy, const QString& name, const QColor& color)
        {
            // Test if energy is nonzero
            if(std::any_of(energy.begin(), energy.end(), [](double e) { return e > 0.0; }))
            {
                auto graph = plot->addGraph();
                graph->setData(parameter, energy);
                graph->setName(name);
                graph->setPen(color);
            }
        };

        // Todo: Get rid of this code repetition (see above)
        add_energy(states.e_pot_limbs, "Limbs (Pot)", QColor(0, 0, 255));
        add_energy(states.e_kin_limbs, "Limbs (Kin)", QColor(40, 40, 255));
        add_energy(states.e_pot_string, "String (Pot)", QColor(128, 0, 128));
        add_energy(states.e_kin_string, "String (Kin)", QColor(128, 40, 128));
        add_energy(states.e_kin_arrow, "Arrow (Kin)", QColor(255, 0, 0));
    }

    plot->rescaleAxes();
    plot->replot();
}
