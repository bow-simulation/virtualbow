#include "EnergyPlot.hpp"
#include <algorithm>
#include <functional>

EnergyPlot::EnergyPlot(const BowStates& states, const std::vector<double>& parameter, const QString& label_x, const UnitGroup& unit_x, const UnitGroup& unit_y)
    : states(states),
      parameter(parameter),
      unit_x(unit_x),
      unit_y(unit_y),
      label_x(label_x),
      plot(new PlotWidget()),
      cb_stacked(new QCheckBox("Stacked")),
      cb_part(new QCheckBox("Group by component")),
      cb_type(new QCheckBox("Group by type"))
{
    plot->setupTopLegend();

    auto vbox = new QVBoxLayout();
    this->setLayout(vbox);
    vbox->setContentsMargins({});
    vbox->setSpacing(0);
    vbox->addWidget(plot, 1);
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

    QObject::connect(&unit_x, &UnitGroup::selectionChanged, this, &EnergyPlot::updatePlot);
    QObject::connect(&unit_y, &UnitGroup::selectionChanged, this, &EnergyPlot::updatePlot);
    updatePlot();
}

void EnergyPlot::setStateIndex(int index) {

}

void EnergyPlot::updatePlot() {
    plot->xAxis->setLabel(label_x + " " + unit_x.getSelectedUnit().getLabel());
    plot->yAxis->setLabel("Energy " + unit_y.getSelectedUnit().getLabel());

    // Function plot_energy adds a single energy to the plot
    std::function<void(const std::vector<double>& energy, const QString& name, const QColor& color)> plot_energy;

    // Select plot_energy and alpha for plotting either stacked or single lines
    if(cb_stacked->isChecked()) {
        std::vector<double> e_lower;
        std::vector<double> e_upper(parameter.size());

        plot_energy = [&, e_lower, e_upper](const std::vector<double>& energy, const QString& name, QColor color) mutable {
            color.setAlpha(155);

            // Test if energy is nonzero
            if(std::none_of(energy.begin(), energy.end(), [](double e) { return e > 0.0; })) {
                return;
            }

            e_lower = e_upper;
            for(size_t i = 0; i < parameter.size(); ++i) {
                e_upper[i] += energy[i];
            }

            auto graph_lower = plot->graph();
            auto graph_upper = plot->addGraph();

            graph_upper->setData(
                unit_x.getSelectedUnit().fromBase(parameter),
                unit_y.getSelectedUnit().fromBase(e_upper)
            );
            graph_upper->setName(name);
            graph_upper->setBrush(color);
            graph_upper->setPen(color);

            if(graph_lower != nullptr) {
                graph_upper->setChannelFillGraph(graph_lower);
            }
        };
    }
    else {
        plot_energy = [&](const std::vector<double>& energy, const QString& name, QColor color) {
            color.setAlpha(255);

            // Test if energy is nonzero
            if(std::none_of(energy.begin(), energy.end(), [](double e) { return e > 0.0; })) {
                return;
            }

            auto graph = plot->addGraph();
            graph->setData(
                unit_x.getSelectedUnit().fromBase(parameter),
                unit_x.getSelectedUnit().fromBase(energy)
            );
            graph->setName(name);
            graph->setPen(color);
        };
    }

    // Clear plot
    plot->clearPlottables();

    // Use plot_energy to plot the energies depending on the grouping option
    if(cb_part->isChecked()) {
        std::vector<double> e_limbs(parameter.size());
        std::vector<double> e_string(parameter.size());

        for(size_t i = 0; i < parameter.size(); ++i) {
            e_limbs[i] = states.e_pot_limbs[i] + states.e_kin_limbs[i];
        }
        for(size_t i = 0; i < parameter.size(); ++i) {
            e_string[i] = states.e_pot_string[i] + states.e_kin_string[i];
        }
        plot_energy(e_limbs, "Limbs (Total)", QColor(0, 0, 255));
        plot_energy(e_string, "String (Total)", QColor(128, 0, 128));
        plot_energy(states.e_kin_arrow, "Arrow (Total)", QColor(255, 0, 0));
    }
    else if(cb_type->isChecked()) {
        std::vector<double> e_pot(parameter.size());
        std::vector<double> e_kin(parameter.size());

        for(size_t i = 0; i < parameter.size(); ++i) {
            e_pot[i] = states.e_pot_limbs[i] + states.e_pot_string[i];
        }
        for(size_t i = 0; i < parameter.size(); ++i) {
            e_kin[i] = states.e_kin_limbs[i] + states.e_kin_string[i] + states.e_kin_arrow[i];
        }

        plot_energy(e_pot, "Potential", QColor(0, 0, 255));
        plot_energy(e_kin, "Kinetic", QColor(255, 0, 0));
    }
    else {
        plot_energy(states.e_pot_limbs, "Limbs (Pot)", QColor(0, 0, 255));
        plot_energy(states.e_kin_limbs, "Limbs (Kin)", QColor(40, 40, 255));
        plot_energy(states.e_pot_string, "String (Pot)", QColor(128, 0, 128));
        plot_energy(states.e_kin_string, "String (Kin)", QColor(128, 40, 128));
        plot_energy(states.e_kin_arrow, "Arrow (Kin)", QColor(255, 0, 0));
    }

    // Update plot
    plot->rescaleAxes(false, true);
    plot->replot();
}
