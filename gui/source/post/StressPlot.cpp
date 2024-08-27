#include "StressPlot.hpp"
#include "pre/limbview/LayerColors.hpp"
#include "pre/viewmodel/units/UnitSystem.hpp"

// Colors from Python's Matplotlib (https://stackoverflow.com/a/42091037)
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

StressPlot::StressPlot(const LimbSetup& limb, const States& states)
    : limb(limb),
      states(states),
      index(0),
      quantity_length(Quantities::length),
      quantity_stress(Quantities::stress)
{
    this->setupTopLegend();

    for(size_t iLayer = 0; iLayer < limb.layers.size(); ++iLayer) {
        QString name = QString::fromStdString("TODO" /*limb.layers[i].name*/);
        QColor color = COLOR_PALETTE[iLayer % COLOR_PALETTE.size()];  // Wrap around when colors are exhausted

        this->addGraph();
        this->graph(2*iLayer)->setName(name + " (back)");
        this->graph(2*iLayer)->setPen({QBrush(color), 2.0, Qt::SolidLine});

        this->addGraph();
        this->graph(2*iLayer+1)->setName(name + " (belly)");
        this->graph(2*iLayer+1)->setPen({QBrush(color), 2.0, Qt::DashLine});
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
    for(size_t iLayer = 0; iLayer < limb.layers.size(); ++iLayer) {
        this->graph(2*iLayer)->data()->clear();
        this->graph(2*iLayer+1)->data()->clear();

        for(size_t iLength = 0; iLength < limb.layers[iLayer].length.size(); ++iLength) {
            this->graph(2*iLayer)->addData(
                quantity_length.getUnit().fromBase(limb.layers[iLayer].length[iLength]),
                quantity_stress.getUnit().fromBase(std::get<0>(states.layer_stress[index][iLayer][iLength]))
            );
            this->graph(2*iLayer+1)->addData(
                quantity_length.getUnit().fromBase(limb.layers[iLayer].length[iLength]),
                quantity_stress.getUnit().fromBase(std::get<1>(states.layer_stress[index][iLayer][iLength]))
            );
        }
    }
}

void StressPlot::updateAxes() {
    this->xAxis->setLabel("Arc length " + quantity_length.getUnit().getLabel());
    this->yAxis->setLabel("Stress " + quantity_stress.getUnit().getLabel());

    QCPRange x_range(
        quantity_length.getUnit().fromBase(limb.length.front()),
        quantity_length.getUnit().fromBase(limb.length.back())
    );
    QCPRange y_range(
        0.0,
        0.0
    );

    for(size_t iState = 0; iState < states.layer_stress.size(); ++iState) {
        for(size_t iLayer = 0; iLayer < states.layer_stress[iState].size(); ++iLayer) {
            for(size_t iLength = 0; iLength < states.layer_stress[iState][iLayer].size(); ++iLength) {
                y_range.expand(quantity_stress.getUnit().fromBase(
                    std::get<0>(states.layer_stress[iState][iLayer][iLength])
                ));
                y_range.expand(quantity_stress.getUnit().fromBase(
                    std::get<1>(states.layer_stress[iState][iLayer][iLength])
                ));
            }
        }
    }

    this->setAxesLimits(x_range, y_range);
}
