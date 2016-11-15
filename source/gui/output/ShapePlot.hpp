#pragma once
#include "../../model/OutputData.hpp"
#include "../Plot.hpp"
#include <QtWidgets>

class ShapePlot: public QWidget
{
public:
    ShapePlot(const BowSetup& setup, const BowStates& states, const std::vector<double>& parameter, const QString& text)
        : states(states),
          parameter(parameter)
    {
        auto vbox = new QVBoxLayout();
        this->setLayout(vbox);

        plot = new Plot("y", "x", Plot::Align::TopLeft);
        plot->fixAspectRatio(true);
        plot->includeOrigin(true, true);
        vbox->addWidget(plot);

        Series limb(setup.limb.y, setup.limb.x);
        plot->addSeries(limb);
        plot->addSeries(limb.flip(false));
        plot->setLinePen(0, QPen(QBrush(Qt::lightGray), 1));
        plot->setLinePen(1, QPen(QBrush(Qt::lightGray), 1));

        // "Stroboscope" plots during different draw lengths
        unsigned steps = 2; // Todo: Magic number
        for(unsigned i = 0; i <= steps; ++i)
        {
            size_t j = i*(parameter.size()-1)/steps;

            Series limb(states.pos_limb_y[j], states.pos_limb_x[j]);
            Series string(states.pos_string_y[j], states.pos_string_x[j]);

            plot->addSeries(limb);
            plot->addSeries(string);
            plot->addSeries(limb.flip(false));
            plot->addSeries(string.flip(false));
            plot->setLinePen(plot->count()-4, QPen(QBrush(Qt::lightGray), 1));
            plot->setLinePen(plot->count()-3, QPen(QBrush(Qt::lightGray), 1));
            plot->setLinePen(plot->count()-2, QPen(QBrush(Qt::lightGray), 1));
            plot->setLinePen(plot->count()-1, QPen(QBrush(Qt::lightGray), 1));
        }

        // Plot at user defined draw length
        plot->addSeries();
        plot->addSeries();
        plot->addSeries();
        plot->addSeries();
        plot->setLinePen(plot->count()-4, QPen(QBrush(Qt::blue), 2));
        plot->setLinePen(plot->count()-3, QPen(QBrush(Qt::blue), 1));
        plot->setLinePen(plot->count()-2, QPen(QBrush(Qt::blue), 2));
        plot->setLinePen(plot->count()-1, QPen(QBrush(Qt::blue), 1));

        auto hbox = new QHBoxLayout();
        vbox->addLayout(hbox);
        hbox->addSpacing(10);   // Todo: Magic number // Todo: Remove the spacing around the plot instead of adding these
        hbox->addWidget(new QLabel(text));

        auto edit = new QLineEdit();
        auto validator = new QDoubleValidator(parameter.front(), parameter.back(), 10); // Todo: Magic number
        validator->setLocale(QLocale::c());
        edit->setValidator(validator);
        hbox->addWidget(edit);
        hbox->addSpacing(15);   // Todo: Magic number

        auto slider = new QSlider(Qt::Horizontal);
        slider->setRange(0, parameter.size()-1);
        hbox->addWidget(slider, 1);
        hbox->addSpacing(10);   // Todo: Magic number // Todo: Remove the spacing around the plot instead of adding these

        // Event handling

        QObject::connect(slider, &QSlider::valueChanged, [this, edit](int index)
        {
            setParameterIndex(index);
            edit->setText(QLocale::c().toString(this->parameter[index]));
        });

        QObject::connect(edit, &QLineEdit::editingFinished, [this, edit, slider]()
        {
            double value = QLocale::c().toDouble(edit->text());
            double min = this->parameter.front();
            double max = this->parameter.back();

            double p = (value - min)/(max - min);
            slider->setValue(double(slider->minimum())*(1.0 - p) + double(slider->maximum())*p);
        });

        emit slider->valueChanged(0);
    }

private:
    const BowStates& states;
    const std::vector<double>& parameter;

    Plot* plot;

    void setParameterIndex(int index)
    {
        Series limb(states.pos_limb_y[index], states.pos_limb_x[index]);
        Series string(states.pos_string_y[index], states.pos_string_x[index]);

        plot->setData(plot->count()-4, limb);
        plot->setData(plot->count()-3, string);
        plot->setData(plot->count()-2, limb.flip(false));
        plot->setData(plot->count()-1, string.flip(false));
        plot->replot();
    }
};
