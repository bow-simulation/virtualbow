#include "StressPlot.hpp"
#include "gui/input/views/LayerColors.hpp"

StressPlot::StressPlot(const InputData& input, const SetupData& setup, const BowStates& states)
    : input(input),
      setup(setup),
      states(states)
{
    this->xAxis->setLabel("Arc length [m]");
    this->yAxis->setLabel("Stress [N/m²]");
    this->setupTopLegend();

    for(int i = 0; i < input.layers.size(); ++i)
    {
        QString name = QString::fromStdString(input.layers[i].name);
        QColor color = getLayerColor(input.layers[i]);

        this->addGraph();
        this->graph(2*i)->setName(name + " (back)");
        this->graph(2*i)->setPen({QBrush(color), 1.0, Qt::SolidLine});

        this->addGraph();
        this->graph(2*i+1)->setName(name + " (belly)");
        this->graph(2*i+1)->setPen({QBrush(color), 1.0, Qt::DashLine});
    }

    setAxesRanges();
}

void StressPlot::setStateIndex(int index)
{
    for(int i = 0; i < setup.limb.layers.size(); ++i)
    {
        const LayerProperties& layer = setup.limb.layers[i];
        this->graph(2*i)->setData(layer.s, layer.sigma_back(states.epsilon[index], states.kappa[index]));
        this->graph(2*i+1)->setData(layer.s, layer.sigma_belly(states.epsilon[index], states.kappa[index]));
    }

    this->replot();
}

#include <QtCore>

void StressPlot::setAxesRanges()
{
    QCPRange x_range(setup.limb.s.minCoeff(), setup.limb.s.maxCoeff());
    QCPRange y_range(0.0, 0.0);

    for(auto& layer: setup.limb.layers)
    {
        for(size_t i = 0; i < states.time.size(); ++i)
        {
            VectorXd sigma_back = layer.sigma_back(states.epsilon[i], states.kappa[i]);
            VectorXd sigma_belly = layer.sigma_belly(states.epsilon[i], states.kappa[i]);

            y_range.expand(sigma_back.minCoeff());
            y_range.expand(sigma_back.maxCoeff());
            y_range.expand(sigma_belly.minCoeff());
            y_range.expand(sigma_belly.maxCoeff());
        }
    }

    this->setAxesLimits(x_range, y_range);
}
