#include "StressPlot.hpp"
#include "gui/limbview/LayerColors.hpp"

StressPlot::StressPlot(const LimbProperties& limb, const BowStates& states)
    : limb(limb),
      states(states),
      index(0),
      unit_length(UnitSystem::length),
      unit_stress(UnitSystem::stress)
{
    this->setupTopLegend();

    for(int i = 0; i < limb.layers.size(); ++i) {
        QString name = QString::fromStdString(limb.layers[i].name);
        QColor layer_color = (limb.layers.size() > 1) ? getLayerColor(limb.layers[i]) : QColor(Qt::blue);
        QColor line_color = QColor::fromHsv(layer_color.hue(), 220, 220);    // Modify saturation and value to make colors better distinguishable

        this->addGraph();
        this->graph(2*i)->setName(name + " (back)");
        this->graph(2*i)->setPen({QBrush(line_color), 1.0, Qt::SolidLine});

        this->addGraph();
        this->graph(2*i+1)->setName(name + " (belly)");
        this->graph(2*i+1)->setPen({QBrush(line_color), 1.0, Qt::DashLine});
    }

    QObject::connect(&unit_length, &UnitGroup::selectionChanged, this, &StressPlot::updatePlot);
    QObject::connect(&unit_stress, &UnitGroup::selectionChanged, this, &StressPlot::updatePlot);
    updatePlot();
}

void StressPlot::setStateIndex(int i) {
    index = i;
    updateStresses();
    this->replot();
}

void StressPlot::updatePlot() {
    updateStresses();
    updateAxes();
    this->replot();
}

void StressPlot::updateStresses() {
    for(int i = 0; i < limb.layers.size(); ++i) {
        const LayerProperties& layer = limb.layers[i];
        this->graph(2*i)->setData(
            unit_length.getSelectedUnit().fromBase(layer.length),
            unit_stress.getSelectedUnit().fromBase(layer.He_back*states.epsilon[index] + layer.Hk_back*states.kappa[index])
        );
        this->graph(2*i+1)->setData(
            unit_length.getSelectedUnit().fromBase(layer.length),
            unit_stress.getSelectedUnit().fromBase(layer.He_belly*states.epsilon[index] + layer.Hk_belly*states.kappa[index])
        );
    }
}

void StressPlot::updateAxes() {
    this->xAxis->setLabel("Arc length " + unit_length.getSelectedUnit().getLabel());
    this->yAxis->setLabel("Stress " + unit_stress.getSelectedUnit().getLabel());

    QCPRange x_range(
        unit_length.getSelectedUnit().fromBase(limb.length.minCoeff()),
        unit_length.getSelectedUnit().fromBase(limb.length.maxCoeff())
    );
    QCPRange y_range(
        0.0,
        0.0
    );

    for(auto& layer: limb.layers)
    {
        for(size_t i = 0; i < states.time.size(); ++i)
        {
            VectorXd sigma_back = layer.He_back*states.epsilon[i] + layer.Hk_back*states.kappa[i];
            VectorXd sigma_belly = layer.He_belly*states.epsilon[i] + layer.Hk_belly*states.kappa[i];

            y_range.expand(unit_stress.getSelectedUnit().fromBase(sigma_back.minCoeff()));
            y_range.expand(unit_stress.getSelectedUnit().fromBase(sigma_back.maxCoeff()));
            y_range.expand(unit_stress.getSelectedUnit().fromBase(sigma_belly.minCoeff()));
            y_range.expand(unit_stress.getSelectedUnit().fromBase(sigma_belly.maxCoeff()));
        }
    }

    this->setAxesLimits(x_range, y_range);
}
