#pragma once
#include "../model/OutputData.hpp"
#include <QtWidgets>

#include "Plot.hpp"

class OutputDialog: public QDialog
{
public:
    OutputDialog(QWidget* parent, const OutputData& output)
        : QDialog(parent)
    {
        auto vbox = new QVBoxLayout();
        this->setLayout(vbox);
        this->setWindowTitle("Simulation results");
        this->resize(800, 600);     // Todo: Magic numbers

        auto plot = new Plot("Draw length", "Draw force");
        vbox->addWidget(plot, 1);
        plot->addSeries();
        plot->setData(0, {output.statics.pos_string, output.statics.draw_force});

        auto btbox = new QDialogButtonBox(QDialogButtonBox::Close);
        vbox->addWidget(btbox);
        QObject::connect(btbox, &QDialogButtonBox::rejected, this, &QDialog::close);
    }
};
