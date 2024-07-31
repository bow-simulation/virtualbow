#include "StressPlot.hpp"
#include "pre/limbview/LayerColors.hpp"

// Colors stolen from Python's Matplotlib
// https://stackoverflow.com/a/42091037
const QList<QColor> COLOR_PALETTE = {
    QColor("#1f77b4"),
    QColor("#ff7f0e"),
    QColor("#2ca02c"),
    QColor("#d62728"),
    QColor("#9467bd"),
    QColor("#8c564b"),
    QColor("#e377c2"),
    QColor("#7f7f7f"),
    QColor("#bcbd22"),
    QColor("#17becf")
};

StressPlot::StressPlot(const LimbProperties& limb, const BowStates& states)
    : limb(limb),
      states(states),
      index(0),
      quantity_length(Quantities::length),
      quantity_stress(Quantities::stress)
{
    this->setupTopLegend();

    for(size_t i = 0; i < limb.layers.size(); ++i) {
        QString name = QString::fromStdString(limb.layers[i].name);
        QColor color = COLOR_PALETTE[i % COLOR_PALETTE.size()];  // Wrap around when colors exceeded

        this->addGraph();
        this->graph(2*i)->setName(name + " (back)");
        this->graph(2*i)->setPen({QBrush(color), 2.0, Qt::SolidLine});

        this->addGraph();
        this->graph(2*i+1)->setName(name + " (belly)");
        this->graph(2*i+1)->setPen({QBrush(color), 2.0, Qt::DashLine});
    }

    QObject::connect(&quantity_length, &Quantity::unitChanged, this, &StressPlot::updatePlot);
    QObject::connect(&quantity_stress, &Quantity::unitChanged, this, &StressPlot::updatePlot);
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
            quantity_length.getUnit().fromBase(layer.length),
            quantity_stress.getUnit().fromBase(layer.He_back*states.epsilon[index] + layer.Hk_back*states.kappa[index])
        );
        this->graph(2*i+1)->setData(
            quantity_length.getUnit().fromBase(layer.length),
            quantity_stress.getUnit().fromBase(layer.He_belly*states.epsilon[index] + layer.Hk_belly*states.kappa[index])
        );
    }
}

void StressPlot::updateAxes() {
    this->xAxis->setLabel("Arc length " + quantity_length.getUnit().getLabel());
    this->yAxis->setLabel("Stress " + quantity_stress.getUnit().getLabel());

    QCPRange x_range(
        quantity_length.getUnit().fromBase(limb.length.minCoeff()),
        quantity_length.getUnit().fromBase(limb.length.maxCoeff())
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

            y_range.expand(quantity_stress.getUnit().fromBase(sigma_back.minCoeff()));
            y_range.expand(quantity_stress.getUnit().fromBase(sigma_back.maxCoeff()));
            y_range.expand(quantity_stress.getUnit().fromBase(sigma_belly.minCoeff()));
            y_range.expand(quantity_stress.getUnit().fromBase(sigma_belly.maxCoeff()));
        }
    }

    this->setAxesLimits(x_range, y_range);
}
