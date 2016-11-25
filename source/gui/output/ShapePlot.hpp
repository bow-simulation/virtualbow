#pragma once
#include "../../model/OutputData.hpp"
#include "../Plot.hpp"

class ShapePlot: public Plot
{
public:
    ShapePlot(const BowSetup& setup, const BowStates& states, bool show_steps)
        : Plot("x", "y", Plot::Align::TopLeft),
          setup(setup),
          states(states)
    {
        if(show_steps)
        {
            plotIntermediateSteps();
        }

        limb_l = this->addSeries({}, Style::Line(Qt::blue, 2));
        limb_r = this->addSeries({}, Style::Line(Qt::blue, 2));
        string_l = this->addSeries({}, Style::Line(Qt::blue));
        string_r = this->addSeries({}, Style::Line(Qt::blue));
        arrow = this->addSeries({}, Style::Scatter(QCPScatterStyle::ssPlus, Qt::red, 10));

        this->setExpansionMode(ExpansionMode::Symmetric, ExpansionMode::OneSided);
        setContentRanges();
    }

    void setStateIndex(int index)
    {
        Series series_limb(states.x_limb[index], states.y_limb[index]);
        this->setData(limb_l, series_limb);
        this->setData(limb_r, series_limb.flip(false));

        Series series_string(states.x_string[index], states.y_string[index]);
        this->setData(string_l, series_string);
        this->setData(string_r, series_string.flip(false));

        Series series_arrow({0.0}, {states.pos_arrow[index]});
        this->setData(arrow, series_arrow);

        this->replot();
    }

private:
    const BowSetup& setup;
    const BowStates& states;

    size_t limb_l;
    size_t limb_r;
    size_t string_l;
    size_t string_r;
    size_t arrow;

    void plotIntermediateSteps()
    {
        // Unbraced state
        Series limb(setup.limb.x, setup.limb.y);
        this->addSeries(limb, Style::Line(Qt::lightGray));
        this->addSeries(limb.flip(false), Style::Line(Qt::lightGray));

        // Stroboscope-like plots during different states
        unsigned steps = 2; // Todo: Magic number
        for(unsigned i = 0; i <= steps; ++i)
        {
            size_t j = i*(states.time.size()-1)/steps;

            Series limb(states.x_limb[j], states.y_limb[j]);
            Series string(states.x_string[j], states.y_string[j]);

            this->addSeries(limb, Style::Line(Qt::lightGray));
            this->addSeries(string, Style::Line(Qt::lightGray));
            this->addSeries(limb.flip(false), Style::Line(Qt::lightGray));
            this->addSeries(string.flip(false), Style::Line(Qt::lightGray));
        }
    }

    void setContentRanges()
    {
        QCPRange rx, ry;
        auto expand = [&](const std::vector<double>& values_x, const std::vector<double>& values_y)
        {
            for(double x: values_x)
            {
                rx.expand(x);
                rx.expand(-x);
            }

            for(double y: values_y)
            {
                ry.expand(y);
            }
        };

        expand(setup.limb.x, setup.limb.y);
        for(size_t i = 0; i < states.time.size(); ++i)
        {
            expand(states.x_limb[i], states.y_limb[i]);
            expand(states.x_string[i], states.y_string[i]);
        }

        Plot::setContentRanges(rx, ry);
    }
};
