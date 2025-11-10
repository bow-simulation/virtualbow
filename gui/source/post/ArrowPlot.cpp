#include "ArrowPlot.hpp"
#include "pre/widgets/PlotWidget.hpp"
#include "pre/models/units/UnitSystem.hpp"

ArrowPlot::ArrowPlot(const Common& common, const States& states)
    : common(common),
      states(states)
{
    posPlot = new PlotWidget();
    velPlot = new PlotWidget();
    accPlot = new PlotWidget();

    posGraph = posPlot->addGraph();
    posGraph->setName("Position");
    posGraph->setPen({Qt::blue, 2.0});

    velGraph = velPlot->addGraph();
    velGraph->setName("Velocity");
    velGraph->setPen({QColor(128, 0, 128), 2.0});

    accGraph = accPlot->addGraph();
    accGraph->setName("Acceleration");
    accGraph->setPen({Qt::red, 2.0});

    auto vbox = new QVBoxLayout();
    vbox->setContentsMargins({});
    vbox->setSpacing(0);
    vbox->addWidget(posPlot);
    vbox->addWidget(velPlot);
    vbox->addWidget(accPlot);
    setLayout(vbox);

    // Connect x-axis ranges of the plots to each other
    QObject::connect(posPlot->xAxis, QOverload<const QCPRange&>::of(&QCPAxis::rangeChanged), this, &ArrowPlot::updateRanges);
    QObject::connect(velPlot->xAxis, QOverload<const QCPRange&>::of(&QCPAxis::rangeChanged), this, &ArrowPlot::updateRanges);
    QObject::connect(accPlot->xAxis, QOverload<const QCPRange&>::of(&QCPAxis::rangeChanged), this, &ArrowPlot::updateRanges);


    // Update plots on unit changes
    QObject::connect(&Quantities::time, &Quantity::unitChanged, this, &ArrowPlot::updatePlots);
    QObject::connect(&Quantities::position, &Quantity::unitChanged, this, &ArrowPlot::updatePlots);
    QObject::connect(&Quantities::velocity, &Quantity::unitChanged, this, &ArrowPlot::updatePlots);
    QObject::connect(&Quantities::acceleration, &Quantity::unitChanged, this, &ArrowPlot::updatePlots);

    // Initialize plots once
    updatePlots();
}

void ArrowPlot::updateRanges(const QCPRange& range) {
    posPlot->xAxis->setRange(range);
    velPlot->xAxis->setRange(range);
    accPlot->xAxis->setRange(range);

    posPlot->replot();
    velPlot->replot();
    accPlot->replot();
}

void ArrowPlot::updatePlots() {
    posPlot->xAxis->setLabel("Time " + Quantities::time.getUnit().getLabel());
    posPlot->yAxis->setLabel("Position " + Quantities::position.getUnit().getLabel());

    velPlot->xAxis->setLabel("Time " + Quantities::time.getUnit().getLabel());
    velPlot->yAxis->setLabel("Velocity " + Quantities::velocity.getUnit().getLabel());

    accPlot->xAxis->setLabel("Time " + Quantities::time.getUnit().getLabel());
    accPlot->yAxis->setLabel("Acceleration " + Quantities::acceleration.getUnit().getLabel());

    posGraph->setData(
        Quantities::time.getUnit().fromBase(states.time),
        Quantities::position.getUnit().fromBase(states.arrow_pos)
    );

    velGraph->setData(
        Quantities::time.getUnit().fromBase(states.time),
        Quantities::velocity.getUnit().fromBase(states.arrow_vel)
    );

    accGraph->setData(
        Quantities::time.getUnit().fromBase(states.time),
        Quantities::acceleration.getUnit().fromBase(states.arrow_acc)
    );

    // Rescale axes without triggering ranges update
    // Include y = 0 and add space in y direction for all
    {
        QSignalBlocker blocker1(posPlot->xAxis);
        posPlot->rescaleAxes(false, true, 1.0, 1.05);

        QSignalBlocker blocker2(velPlot->xAxis);
        velPlot->rescaleAxes(false, true, 1.0, 1.05);

        QSignalBlocker blocker3(accPlot->xAxis);
        accPlot->rescaleAxes(false, true, 1.0, 1.05);
    }


    posPlot->replot();
    velPlot->replot();
    accPlot->replot();
}
