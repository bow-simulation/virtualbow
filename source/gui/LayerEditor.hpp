#pragma once
#include <QtWidgets>
#include "Plot.hpp"

class LayerEditor: public QWidget
{
public:
    LayerEditor()
    {
        list = new QListWidget;

        plot1 = new Plot;
        plot2 = new Plot;

        // Layout
        QVBoxLayout* v = new QVBoxLayout;
        v->addWidget(list);

        this->setLayout(v);
    }

private:
    QListWidget* list;
    Plot plot1;
    Plot plot2;
};
