#pragma once
#include "../../model/OutputData.hpp"
#include "../Plot.hpp"
#include <QtWidgets>

class ShapePlot: public Plot
{
public:
    ShapePlot(const BowSetup& setup, const BowStates& states, bool show_steps)
        : Plot("x", "y", Plot::Align::TopLeft),
          states(states)
    {
        this->fixAspectRatio(true);
        this->includeOrigin(true, true);

        QPen pen_steps = show_steps ? QPen(QBrush(Qt::lightGray), 1) : Qt::NoPen;

        Series limb(setup.limb.x, setup.limb.y);
        this->addSeries(limb);
        this->addSeries(limb.flip(false));
        this->setLinePen(0, pen_steps);
        this->setLinePen(1, pen_steps);

        // "Stroboscope" plots during different draw lengths
        unsigned steps = 2; // Todo: Magic number
        for(unsigned i = 0; i <= steps; ++i)
        {
            size_t j = i*(states.time.size()-1)/steps;

            Series limb(states.pos_limb_x[j], states.pos_limb_y[j]);
            Series string(states.pos_string_x[j], states.pos_string_y[j]);

            this->addSeries(limb);
            this->addSeries(string);
            this->addSeries(limb.flip(false));
            this->addSeries(string.flip(false));
            this->setLinePen(this->count()-4, pen_steps);
            this->setLinePen(this->count()-3, pen_steps);
            this->setLinePen(this->count()-2, pen_steps);
            this->setLinePen(this->count()-1, pen_steps);
        }

        // Plot at user defined draw length
        index_limb_a = this->addSeries();
        index_limb_b = this->addSeries();
        index_string_a = this->addSeries();
        index_string_b = this->addSeries();
        index_arrow = this->addSeries();

        this->setLinePen(index_limb_a, QPen(QBrush(Qt::blue), 2));
        this->setLinePen(index_limb_b, QPen(QBrush(Qt::blue), 2));
        this->setLinePen(index_string_a, QPen(QBrush(Qt::blue), 1));
        this->setLinePen(index_string_b, QPen(QBrush(Qt::blue), 1));
        this->setLineStyle(index_arrow, QCPCurve::lsNone);
        this->setScatterStyle(index_arrow, QCPScatterStyle(QCPScatterStyle::ssPlus, Qt::red, 8));

    }

    void setShapeIndex(int index)
    {
        Series limb(states.pos_limb_x[index], states.pos_limb_y[index]);
        Series string(states.pos_string_x[index], states.pos_string_y[index]);
        Series arrow({0.0}, {states.pos_arrow[index]});

        this->setData(index_limb_a, limb);
        this->setData(index_limb_b, limb.flip(false));
        this->setData(index_string_a, string);
        this->setData(index_string_b, string.flip(false));
        this->setData(index_arrow, arrow);
        this->replot();
    }

private:
    const BowStates& states;

    size_t index_limb_a;
    size_t index_limb_b;
    size_t index_string_a;
    size_t index_string_b;
    size_t index_arrow;
};
