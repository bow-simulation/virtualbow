#include "EnergyPlot.hpp"
#include "pre/Language.hpp"
#include <algorithm>
#include <functional>

EnergyPlot::EnergyPlot(const States& states, const std::vector<double>& parameter, const QString& label_x, const Quantity& quantity_x, const Quantity& quantity_y)
    : states(states),
      parameter(parameter),
      quantity_x(quantity_x),
      quantity_y(quantity_y),
      label_x(label_x)
{
    plot = new PlotWidget();
    plot->setupTopLegend();

    cb_stacked = new QCheckBox("Stacked");
    cb_stacked->setToolTip(Tooltips::EnergiesStacked);

    cb_part = new QCheckBox("Group by component");
    cb_part->setToolTip(Tooltips::EnergiesByPart);

    cb_type = new QCheckBox("Group by energy");
    cb_type->setToolTip(Tooltips::EnergiesByType);

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

    QObject::connect(&quantity_x, &Quantity::unitChanged, this, &EnergyPlot::updatePlot);
    QObject::connect(&quantity_y, &Quantity::unitChanged, this, &EnergyPlot::updatePlot);
    updatePlot();
}

void EnergyPlot::setStateIndex(int index) {

}

void EnergyPlot::updatePlot() {
    plot->xAxis->setLabel(label_x + " " + quantity_x.getUnit().getLabel());
    plot->yAxis->setLabel("Energy " + quantity_y.getUnit().getLabel());

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
                quantity_x.getUnit().fromBase(parameter),
                quantity_y.getUnit().fromBase(e_upper)
            );
            graph_upper->setName(name);
            graph_upper->setBrush(color);
            graph_upper->setPen({QBrush(color), 2.0});

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
                quantity_x.getUnit().fromBase(parameter),
                quantity_x.getUnit().fromBase(energy)
            );
            graph->setName(name);
            graph->setPen({QBrush(color), 2.0});
        };
    }

    // Clear plot
    plot->clearPlottables();

    // Use plot_energy to plot the energies depending on the grouping option
    if(cb_part->isChecked()) {
        std::vector<double> e_limbs(parameter.size());
        for(size_t i = 0; i < parameter.size(); ++i) {
            e_limbs[i] = states.elastic_energy_limbs[i] + states.kinetic_energy_limbs[i];
        }

        std::vector<double> e_string(parameter.size());
        for(size_t i = 0; i < parameter.size(); ++i) {
            e_string[i] = states.elastic_energy_string[i] + states.kinetic_energy_string[i];
        }

        std::vector<double> e_damp(parameter.size());
        for(size_t i = 0; i < parameter.size(); ++i) {
            e_damp[i] = states.damping_energy_limbs[i] + states.damping_energy_string[i];
        }

        plot_energy(e_limbs, "Limbs\n(pot + kin)", QColor(0, 0, 255));
        plot_energy(e_string, "String\n(pot + kin)", QColor(128, 0, 128));
        plot_energy(states.kinetic_energy_arrow, "Arrow\n(pot + kin)", QColor(255, 0, 0));
        plot_energy(e_damp, "Damping", QColor(128, 128, 128));
    }
    else if(cb_type->isChecked()) {
        std::vector<double> e_pot(parameter.size());
        for(size_t i = 0; i < parameter.size(); ++i) {
            e_pot[i] = states.elastic_energy_limbs[i] + states.elastic_energy_string[i];
        }

        std::vector<double> e_kin(parameter.size());
        for(size_t i = 0; i < parameter.size(); ++i) {
            e_kin[i] = states.kinetic_energy_limbs[i] + states.kinetic_energy_string[i] + states.kinetic_energy_arrow[i];
        }

        std::vector<double> e_damp(parameter.size());
        for(size_t i = 0; i < parameter.size(); ++i) {
            e_damp[i] = states.damping_energy_limbs[i] + states.damping_energy_string[i];
        }

        plot_energy(e_pot, "Potential", QColor(0, 0, 255));
        plot_energy(e_kin, "Kinetic", QColor(255, 0, 0));
        plot_energy(e_damp, "Damping", QColor(128, 128, 128));
    }
    else {
        plot_energy(states.elastic_energy_limbs, "Limbs\n(potential)", QColor(0, 0, 255));
        plot_energy(states.kinetic_energy_limbs, "Limbs\n(kinetic)", QColor(40, 40, 255));
        plot_energy(states.elastic_energy_string, "String\n(potential)", QColor(128, 0, 128));
        plot_energy(states.kinetic_energy_string, "String\n(kinetic)", QColor(128, 40, 128));
        plot_energy(states.kinetic_energy_arrow, "Arrow\n(kinetic)", QColor(255, 0, 0));
        plot_energy(states.damping_energy_limbs, "Limbs\n(damping)", QColor(128, 128, 128));
        plot_energy(states.damping_energy_string, "String\n(damping)", QColor(179, 179, 179));
    }

    // Update plot
    plot->rescaleAxes(false, true);
    plot->replot();
}
