#include "DrawForcePlot.hpp"
#include "pre/widgets/PlotWidget.hpp"
#include "pre/models/units/UnitSystem.hpp"
#include "pre/Language.hpp"

DrawForcePlot::DrawForcePlot(const Common& common, const States& states)
    : common(common),
      states(states)
{
    plot = new PlotWidget();
    plot->setupTopLegend();

    forceGraph = plot->addGraph(plot->xAxis, plot->yAxis);
    forceGraph->setName("Force");
    forceGraph->setPen({Qt::blue, 2.0});

    lineGraph = plot->addGraph(plot->xAxis, plot->yAxis);
    lineGraph->setName("Linear");
    lineGraph->setPen({Qt::blue, 1.0});

    stiffnessGraph = plot->addGraph(plot->xAxis, plot->yAxis2);
    stiffnessGraph->setName("Stiffness");
    stiffnessGraph->setPen({QColor(128, 0, 128), 2.0});

    cbLine = new QCheckBox("Show linear reference");
    cbLine->setToolTip(Tooltips::DrawForceShowLine);

    cbStiffness = new QCheckBox("Show stiffness");
    cbStiffness->setToolTip(Tooltips::DrawForceShowStiffness);

    auto hbox = new QHBoxLayout();
    hbox->addStretch();
    hbox->addWidget(cbLine);
    hbox->addSpacing(20);
    hbox->addWidget(cbStiffness);
    hbox->addStretch();

    auto vbox = new QVBoxLayout();
    vbox->setContentsMargins({});
    vbox->setSpacing(0);
    vbox->addWidget(plot, 1);
    vbox->addSpacing(10);
    vbox->addLayout(hbox);
    vbox->addSpacing(10);
    setLayout(vbox);

    QObject::connect(cbLine, &QCheckBox::toggled, this, &DrawForcePlot::updateVisibility);
    QObject::connect(cbStiffness, &QCheckBox::toggled, this, &DrawForcePlot::updateVisibility);

    QObject::connect(&Quantities::length, &Quantity::unitChanged, this, &DrawForcePlot::updateGraphs);
    QObject::connect(&Quantities::force, &Quantity::unitChanged, this, &DrawForcePlot::updateGraphs);
    QObject::connect(&Quantities::stiffness, &Quantity::unitChanged, this, &DrawForcePlot::updateGraphs);

    updateVisibility();
    updateGraphs();
}

void DrawForcePlot::updateVisibility() {
    lineGraph->setVisible(cbLine->isChecked());
    plot->legend->itemWithPlottable(lineGraph)->setVisible(cbLine->isChecked());

    stiffnessGraph->setVisible(cbStiffness->isChecked());
    plot->legend->itemWithPlottable(stiffnessGraph)->setVisible(cbStiffness->isChecked());

    plot->yAxis2->setLabelColor(cbStiffness->isChecked() ? Qt::black : Qt::transparent);
    plot->yAxis2->setTickLabels(cbStiffness->isChecked());
    plot->yAxis2->setTicks(cbStiffness->isChecked());

    plot->replot();
}

void DrawForcePlot::updateGraphs() {
    plot->xAxis->setLabel("Draw length " + Quantities::length.getUnit().getLabel());
    plot->yAxis->setLabel("Draw force " + Quantities::force.getUnit().getLabel());
    plot->yAxis2->setLabel("Draw stiffness " + Quantities::stiffness.getUnit().getLabel());

    forceGraph->setData(
        Quantities::length.getUnit().fromBase(states.draw_length),
        Quantities::force.getUnit().fromBase(states.draw_force)
    );

    lineGraph->setData(
        Quantities::length.getUnit().fromBase({states.draw_length.front(), states.draw_length.back()}),
        Quantities::force.getUnit().fromBase({states.draw_force.front(), states.draw_force.back()})
    );

    stiffnessGraph->setData(
        Quantities::length.getUnit().fromBase(states.draw_length),
        Quantities::stiffness.getUnit().fromBase(states.draw_stiffness)
    );

    plot->rescaleAxes();
    plot->replot();
}
