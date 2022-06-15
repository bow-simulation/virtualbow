#include "ComboPlot.hpp"

ComboPlot::ComboPlot()
    : combo_x(new QComboBox()),
      combo_y(new QComboBox()),
      plot(new PlotWidget()),
      curve(new QCPCurve(plot->xAxis, plot->yAxis))
{
    curve->setName("Line");
    curve->setPen({Qt::blue, 2.0});

    auto vbox = new QVBoxLayout();
    this->setLayout(vbox);
    vbox->setContentsMargins({});
    vbox->setSpacing(0);
    vbox->addWidget(plot, 1);
    vbox->addSpacing(10);

    auto hbox = new QHBoxLayout();
    vbox->addLayout(hbox);
    vbox->addSpacing(10);

    hbox->addStretch(1);
    hbox->addWidget(new QLabel("X Axis:"));
    hbox->addSpacing(10);
    hbox->addWidget(combo_x, 1);
    hbox->addSpacing(20);
    hbox->addWidget(new QLabel("Y Axis:"));
    hbox->addSpacing(10);
    hbox->addWidget(combo_y, 1);
    hbox->addStretch(1);

    QObject::connect(combo_x, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &ComboPlot::updatePlot);
    QObject::connect(combo_y, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &ComboPlot::updatePlot);
}

void ComboPlot::addData(const QString& name, const std::vector<double>& data, const UnitGroup& unit) {
    this->data.append(&data);
    this->units.append(&unit);

    const QSignalBlocker blocker_x(combo_x);
    combo_x->addItem(name);

    const QSignalBlocker blocker_y(combo_y);
    combo_y->addItem(name);

    // If the unit changes, update the plot only if the changing unit is currently selected
    auto conditional_update = [&]{
        if(units[combo_x->currentIndex()] == &unit || units[combo_y->currentIndex()] == &unit) {
            updatePlot();
        }
    };
    QObject::connect(&unit, &UnitGroup::selectionChanged, this, conditional_update);
    QObject::connect(&unit, &UnitGroup::selectionChanged, this, conditional_update);
}

void ComboPlot::setCombination(int index_x, int index_y) {
    combo_x->setCurrentIndex(index_x);
    combo_y->setCurrentIndex(index_y);
}

void ComboPlot::updatePlot() {
    int index_x = combo_x->currentIndex();
    int index_y = combo_y->currentIndex();

    Unit unit_x = units[index_x]->getSelectedUnit();
    Unit unit_y = units[index_y]->getSelectedUnit();

    plot->xAxis->setLabel(combo_x->currentText() + " " + unit_x.getLabel());
    plot->yAxis->setLabel(combo_y->currentText() + " " + unit_y.getLabel());

    curve->setData(
        unit_x.fromBase(*data[index_x]),
        unit_y.fromBase(*data[index_y])
    );

    plot->rescaleAxes();
    plot->replot();
}
