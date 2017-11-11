#include "EnergyPlot.hpp"
#include "gui/HorizontalLine.hpp"
#include <algorithm>
#include <functional>

EnergyPlot::EnergyPlot(const BowStates& states, const std::vector<double>& parameter, const QString& x_label)
    : states(states),
      parameter(parameter),
      plot(new PlotWidget()),
      cb_stacked(new QCheckBox("Stacked")),
      cb_part(new QCheckBox("Group by component")),
      cb_type(new QCheckBox("Group by type"))
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
    hbox->addWidget(cb_stacked);
    hbox->addSpacing(20);    // Magic number
    hbox->addWidget(cb_part);
    hbox->addSpacing(20);    // Magic number
    hbox->addWidget(cb_type);
    hbox->addStretch();

    cb_stacked->setChecked(true);

    QObject::connect(cb_part, &QCheckBox::clicked, [&]{
        cb_type->setChecked(false);
    });

    QObject::connect(cb_type, &QCheckBox::clicked, [&]{
        cb_part->setChecked(false);
    });

    QObject::connect(cb_stacked, &QCheckBox::toggled, this, &EnergyPlot::updatePlot);
    QObject::connect(cb_type, &QCheckBox::toggled, this, &EnergyPlot::updatePlot);
    QObject::connect(cb_part, &QCheckBox::toggled, this, &EnergyPlot::updatePlot);


    updatePlot();
}

void EnergyPlot::setStateIndex(int index)
{

}

/*
void EnergyPlot::updatePlot()
{
    plot->clearPlottables();

    if(cb_stacked->isChecked())
        plotStacked();
    else
        plotLines();

    plot->rescaleAxes();
    plot->replot();
}
*/

void EnergyPlot::updatePlot()
{
    // Function plot_energy adds a single energy to the plot
    std::function<void(const std::vector<double>& energy, const QString& name, const QColor& color)> plot_energy;
    int alpha;    // Transparency

    // Select plot_energy for plotting either stacked or single lines
    if(cb_stacked->isChecked())
    {
        std::vector<double> e_lower, e_upper(parameter.size());
        plot_energy = [&, e_lower, e_upper](const std::vector<double>& energy, const QString& name, const QColor& color) mutable
        {
            // Test if energy is nonzero
            if(std::none_of(energy.begin(), energy.end(), [](double e) { return e > 0.0; }))
                return;

            e_lower = e_upper;
            for(size_t i = 0; i < parameter.size(); ++i)
            {
                e_upper[i] += energy[i];
            }

            auto graph_lower = plot->graph();
            auto graph_upper = plot->addGraph();

            graph_upper->setData(parameter, e_upper);
            graph_upper->setName(name);
            graph_upper->setBrush(color);
            graph_upper->setPen(color);

            if(graph_lower != nullptr)
            {
                graph_upper->setChannelFillGraph(graph_lower);
            }
        };

        alpha = 155;
    }
    else
    {
        plot_energy = [&](const std::vector<double>& energy, const QString& name, const QColor& color)
        {
            // Test if energy is nonzero
            if(std::none_of(energy.begin(), energy.end(), [](double e) { return e > 0.0; }))
                return;

            auto graph = plot->addGraph();
            graph->setData(parameter, energy);
            graph->setName(name);
            graph->setPen(color);
        };

        alpha = 0;
    }

    // Clear plot
    plot->clearPlottables();

    // Use plot_energy to plot the energies depending on the grouping option
    if(cb_part->isChecked())
    {
        std::vector<double> e_limbs(parameter.size());
        std::vector<double> e_string(parameter.size());

        for(size_t i = 0; i < parameter.size(); ++i)
            e_limbs[i] = states.e_pot_limbs[i] + states.e_kin_limbs[i];

        for(size_t i = 0; i < parameter.size(); ++i)
            e_string[i] = states.e_pot_string[i] + states.e_kin_string[i];

        plot_energy(e_limbs, "Limbs (Total)", QColor(0, 0, 255, alpha));
        plot_energy(e_string, "String (Total)", QColor(128, 0, 128, alpha));
        plot_energy(states.e_kin_arrow, "Arrow (Total)", QColor(255, 0, 0, alpha));
    }
    else if(cb_type->isChecked())
    {
        std::vector<double> e_pot(parameter.size());
        std::vector<double> e_kin(parameter.size());

        for(size_t i = 0; i < parameter.size(); ++i)
            e_pot[i] = states.e_pot_limbs[i] + states.e_pot_string[i];

        for(size_t i = 0; i < parameter.size(); ++i)
            e_kin[i] = states.e_kin_limbs[i] + states.e_kin_string[i] + states.e_kin_arrow[i];

        plot_energy(e_pot, "Potential", QColor(0, 0, 255, alpha));
        plot_energy(e_kin, "Kinetic", QColor(255, 0, 0, alpha));
    }
    else
    {
        plot_energy(states.e_pot_limbs, "Limbs (Pot)", QColor(0, 0, 255, alpha));
        plot_energy(states.e_kin_limbs, "Limbs (Kin)", QColor(40, 40, 255, alpha));
        plot_energy(states.e_pot_string, "String (Pot)", QColor(128, 0, 128, alpha));
        plot_energy(states.e_kin_string, "String (Kin)", QColor(128, 40, 128, alpha));
        plot_energy(states.e_kin_arrow, "Arrow (Kin)", QColor(255, 0, 0, alpha));
    }

    // Update plot
    plot->rescaleAxes();
    plot->replot();
}
