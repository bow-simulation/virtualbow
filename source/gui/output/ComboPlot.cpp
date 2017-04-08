#include "ComboPlot.hpp"
#include "gui/HorizontalLine.hpp"

// http://stackoverflow.com/questions/23139820/how-to-get-data-back-from-qvariant-for-a-usertype
Q_DECLARE_METATYPE(const std::vector<double>*)

ComboPlot::ComboPlot()
    : combo_x(new QComboBox()),
      combo_y(new QComboBox()),
      plot(new PlotWidget()),
      curve(new QCPCurve(plot->xAxis, plot->yAxis))
{
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

    hbox->addStretch(1);
    hbox->addWidget(new QLabel("X-Axis:"));
    hbox->addSpacing(10);    // Magic number
    hbox->addWidget(combo_x, 1);
    hbox->addSpacing(20);    // Magic number
    hbox->addWidget(new QLabel("Y-Axis:"));
    hbox->addSpacing(10);    // Magic number
    hbox->addWidget(combo_y, 1);
    hbox->addStretch(1);

    /*
    auto hbox = new QHBoxLayout();
    vbox->addLayout(hbox);
    vbox->addSpacing(vbox->spacing());
    hbox->addStretch();
    hbox->addWidget(new QCheckBox("Stacked"));
    hbox->addSpacing(20);    // Magic number
    hbox->addWidget(new QCheckBox("Group by energy"));
    hbox->addSpacing(20);    // Magic number
    hbox->addWidget(new QCheckBox("Group by part"));
    hbox->addStretch();
    */

    // http://doc.qt.io/qt-5/qcombobox.html#currentIndexChanged
    QObject::connect(combo_x, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &ComboPlot::updatePlot);
    QObject::connect(combo_y, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &ComboPlot::updatePlot);
}

void ComboPlot::addData(const QString& name, const std::vector<double>& data)
{
    // Todo: Why two steps?
    QVariant v;
    v.setValue(&data);

    // Todo: Do this "right" by remembering the old block state
    combo_x->blockSignals(true);
    combo_y->blockSignals(true);

    combo_x->addItem(name, v);
    combo_y->addItem(name, v);

    combo_x->blockSignals(false);
    combo_y->blockSignals(false);
}

void ComboPlot::setCombination(int index_x, int index_y)
{
    combo_x->setCurrentIndex(index_x);
    combo_y->setCurrentIndex(index_y);
}

void ComboPlot::updatePlot()
{
    // http://stackoverflow.com/questions/23139820/how-to-get-data-back-from-qvariant-for-a-usertype
    auto x = combo_x->currentData().value<const std::vector<double>*>();
    auto y = combo_y->currentData().value<const std::vector<double>*>();

    curve->setData(*x, *y);
    plot->xAxis->setLabel(combo_x->currentText());
    plot->yAxis->setLabel(combo_y->currentText());
    plot->rescaleAxes();
    plot->replot();
}
