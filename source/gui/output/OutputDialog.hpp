#pragma once
#include "../../model/OutputData.hpp"
#include "../Plot.hpp"
#include "OutputGrid.hpp"
#include "ShapePlot.hpp"
#include "StressPlot.hpp"
#include "EnergyPlot.hpp"
#include "Slider.hpp"
#include <QtWidgets>

class StaticOutput: public QWidget
{
public:
    StaticOutput(const BowSetup& setup, const BowStates& statics)
    {
        auto vbox = new QVBoxLayout();
        this->setLayout(vbox);

        auto grid = new OutputGrid();
        grid->add(0, 0, "String length:", 0.0);
        grid->add(1, 0, "Draw force:", 0.0);
        grid->add(0, 1, "Energy braced:", 0.0);
        grid->add(1, 1, "Energy drawn:", 0.0);
        grid->add(0, 2, "Drawing work:", 0.0);
        grid->add(1, 2, "Storage ratio:", 0.0);
        grid->add(0, 3, "Max. Tension:", 0.0);
        grid->add(1, 3, "Max. Compression:", 0.0);
        vbox->addWidget(grid);

        auto plot_shapes = new ShapePlot(setup, statics, true);
        auto plot_stress = new StressPlot(setup, statics);
        auto plot_energy = new EnergyPlot(statics, statics.draw_length, "Draw length");

        auto tabs = new QTabWidget();
        tabs->addTab(plot_shapes, "Shape");
        tabs->addTab(plot_stress, "Stress");
        tabs->addTab(plot_energy, "Energy");
        vbox->addWidget(tabs);

        auto slider = new Slider(statics.draw_length, "Draw length:");
        QObject::connect(slider, &Slider::valueChanged, plot_shapes, &ShapePlot::setStateIndex);
        QObject::connect(slider, &Slider::valueChanged, plot_stress, &StressPlot::setStateIndex);
        QObject::connect(slider, &Slider::valueChanged, plot_energy, &EnergyPlot::setStateIndex);
        emit slider->valueChanged(0);
        vbox->addWidget(slider);
    }
};

class DynamicOutput: public QWidget
{
public:
    DynamicOutput(const BowSetup& setup, const BowStates& dynamics)
    {
        auto vbox = new QVBoxLayout();
        this->setLayout(vbox);

        auto grid = new OutputGrid();
        grid->add(0, 0, "Arrow velocity:", 0.0);
        grid->add(1, 0, "Arrow energy:", 0.0);
        grid->add(0, 1, "Efficiency:", 0.0);
        vbox->addWidget(grid);

        auto plot_shapes = new ShapePlot(setup, dynamics, false);
        auto plot_stress = new StressPlot(setup, dynamics);
        auto plot_energy = new EnergyPlot(dynamics, dynamics.time, "Time");

        auto tabs = new QTabWidget();
        tabs->addTab(plot_shapes, "Shape");
        tabs->addTab(plot_stress, "Stress");
        tabs->addTab(plot_energy, "Energy");
        vbox->addWidget(tabs);

        auto slider = new Slider(dynamics.time, "Time:");
        QObject::connect(slider, &Slider::valueChanged, plot_shapes, &ShapePlot::setStateIndex);
        QObject::connect(slider, &Slider::valueChanged, plot_stress, &StressPlot::setStateIndex);
        QObject::connect(slider, &Slider::valueChanged, plot_energy, &EnergyPlot::setStateIndex);
        emit slider->valueChanged(0);
        vbox->addWidget(slider);
    }
};

class OutputDialog: public QDialog
{
public:
    OutputDialog(QWidget* parent, const OutputData& output)
        : QDialog(parent)
    {
        auto vbox = new QVBoxLayout();
        this->setLayout(vbox);
        this->setWindowTitle("Simulation results");
        this->setWindowFlags(this->windowFlags() | Qt::WindowMaximizeButtonHint);
        this->resize(1000, 800);     // Todo: Magic numbers

        auto tabs = new QTabWidget();
        tabs->addTab(output.statics ? new StaticOutput(output.setup, *output.statics) : new QWidget(), "Statics");
        tabs->addTab(output.dynamics ? new DynamicOutput(output.setup, *output.dynamics) : new QWidget(), "Dynamics");
        tabs->setTabEnabled(0, output.statics != nullptr);
        tabs->setTabEnabled(1, output.dynamics != nullptr);
        tabs->setIconSize({24, 24});    // Todo: Magic numbers
        tabs->setDocumentMode(true);
        vbox->addWidget(tabs, 1);

        // Todo: Better solution?
        // When a tab is set as disabled the icons are somehow converted to grayscale internally.
        // Look at Qt source how they do that, maybe there is no need to have an extra grey version of the icon.
        auto set_icons = [tabs](int index)
        {
            switch(index)
            {
            case 0:
                tabs->setTabIcon(0, QIcon(":/icons/circle-yellow"));
                tabs->setTabIcon(1, QIcon(":/icons/circle-grey"));
                break;
            case 1:
                tabs->setTabIcon(0, QIcon(":/icons/circle-grey"));
                tabs->setTabIcon(1, QIcon(":/icons/circle-green"));
                break;
            }
        };

        QObject::connect(tabs, &QTabWidget::currentChanged, set_icons);
        set_icons(0);

        auto btbox = new QDialogButtonBox(QDialogButtonBox::Close);
        QObject::connect(btbox, &QDialogButtonBox::rejected, this, &QDialog::close);
        vbox->addWidget(btbox);
    }

private:
    // Keep dialog from closing on enter
    // http://stackoverflow.com/questions/15845487/how-do-i-prevent-the-enter-key-from-closing-my-qdialog-qt-4-8-1
    virtual void keyPressEvent(QKeyEvent *evt) override
    {
        if(evt->key() == Qt::Key_Enter || evt->key() == Qt::Key_Return)
            return;

        QDialog::keyPressEvent(evt);
    }
};
