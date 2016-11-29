#pragma once
#include "../../model/OutputData.hpp"
#include "../Plot.hpp"

// http://stackoverflow.com/questions/23139820/how-to-get-data-back-from-qvariant-for-a-usertype
Q_DECLARE_METATYPE(const std::vector<double>*)

class ComboPlot: public QWidget
{
public:
    ComboPlot()
        : combo_x(new QComboBox()),
          combo_y(new QComboBox()),
          plot(new Plot())
    {
        auto vbox = new QVBoxLayout();
        this->setLayout(vbox);

        auto hbox = new QHBoxLayout();
        vbox->addLayout(hbox);
        hbox->addSpacing(10);           // Todo: Get rid of this and the margin of 10 around the plot. Where does it come from?
        hbox->addWidget(new QLabel("x-Axis:"), 0);
        hbox->addWidget(combo_x, 1);
        hbox->addSpacing(10);
        hbox->addWidget(new QLabel("y-Axis:"), 0);
        hbox->addWidget(combo_y, 1);
        hbox->addSpacing(10);           // Todo: Get rid of this and the margin of 10 around the plot. Where does it come from?

        plot->addSeries();
        vbox->addWidget(plot, 1);

        // http://doc.qt.io/qt-5/qcombobox.html#currentIndexChanged
        QObject::connect(combo_x, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &ComboPlot::setPlotData);
        QObject::connect(combo_y, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &ComboPlot::setPlotData);
    }

    void addData(const QString& name, const std::vector<double>& data)
    {
        // Todo: Why two steps?
        QVariant v;
        v.setValue(&data);

        combo_x->blockSignals(true);
        combo_y->blockSignals(true);

        combo_x->addItem(name, v);
        combo_y->addItem(name, v);

        combo_x->blockSignals(false);
        combo_y->blockSignals(false);
    }

    void setCombination(int index_x, int index_y)
    {
        combo_x->setCurrentIndex(index_x);
        combo_y->setCurrentIndex(index_y);
    }

private:
    QComboBox* combo_x;
    QComboBox* combo_y;
    Plot* plot;

    void setPlotData(int index)
    {
        // http://stackoverflow.com/questions/23139820/how-to-get-data-back-from-qvariant-for-a-usertype
        auto x = combo_x->currentData().value<const std::vector<double>*>();
        auto y = combo_y->currentData().value<const std::vector<double>*>();
        plot->setData(0, {*x, *y});
        plot->setLabels(combo_x->currentText(), combo_y->currentText());
        plot->fitContent(true, true);
        plot->replot();
    }
};
