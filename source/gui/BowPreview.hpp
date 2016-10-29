#pragma once
#include "Plot.hpp"

#include <QtWidgets>

class BowPreview: public QWidget
{
public:
    BowPreview()
    {
        auto plot1 = new Plot();
        auto plot2 = new Plot();

        auto box = new QHBoxLayout();
        this->setLayout(box);
        box->addWidget(plot1);
        box->addWidget(plot2);
    }

};
