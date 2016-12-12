#include "OutputDialog.hpp"
#include "../Plot.hpp"
#include "OutputGrid.hpp"
#include "ShapePlot.hpp"
#include "StressPlot.hpp"
#include "EnergyPlot.hpp"
#include "ComboPlot.hpp"
#include "Slider.hpp"

StaticOutput::StaticOutput(const BowSetup& setup, const StaticData& statics)
{
    auto vbox = new QVBoxLayout();
    this->setLayout(vbox);

    auto grid = new OutputGrid();
    grid->add(0, 0, "String length:", "m", setup.string_length);
    grid->add(1, 0, "Final draw force:", "N", statics.final_draw_force);
    grid->add(0, 2, "Drawing work:", "J", statics.drawing_work);
    grid->add(1, 2, "Storage ratio:", "", statics.storage_ratio);
    vbox->addWidget(grid);

    auto plot_shapes = new ShapePlot(setup, statics.states, true);
    auto plot_stress = new StressPlot(setup, statics.states);
    auto plot_energy = new EnergyPlot(statics.states, statics.states.draw_length, "Draw length [m]");
    auto plot_combo = new ComboPlot();
    plot_combo->addData("Draw length [m]", statics.states.draw_length);
    plot_combo->addData("Draw force [N]", statics.states.draw_force);
    plot_combo->setCombination(0, 1);

    auto tabs = new QTabWidget();
    tabs->addTab(plot_shapes, "Shape");
    tabs->addTab(plot_stress, "Stress");
    tabs->addTab(plot_energy, "Energy");
    tabs->addTab(plot_combo, "Other");
    vbox->addWidget(tabs);

    auto slider = new Slider(statics.states.draw_length, "Draw length [m]:");
    QObject::connect(slider, &Slider::valueChanged, plot_shapes, &ShapePlot::setStateIndex);
    QObject::connect(slider, &Slider::valueChanged, plot_stress, &StressPlot::setStateIndex);
    QObject::connect(slider, &Slider::valueChanged, plot_energy, &EnergyPlot::setStateIndex);
    emit slider->valueChanged(0);
    vbox->addWidget(slider);
}

DynamicOutput::DynamicOutput(const BowSetup& setup, const DynamicData& dynamics)
{
    auto vbox = new QVBoxLayout();
    this->setLayout(vbox);

    auto grid = new OutputGrid();
    grid->add(0, 0, "Final arrow velocity:", "m/s", dynamics.final_arrow_velocity);
    grid->add(1, 0, "Final arrow energy:", "J", dynamics.final_arrow_energy);
    grid->add(0, 1, "Efficiency:", "", dynamics.efficiency);
    vbox->addWidget(grid);

    auto plot_shapes = new ShapePlot(setup, dynamics.states, false);
    auto plot_stress = new StressPlot(setup, dynamics.states);
    auto plot_energy = new EnergyPlot(dynamics.states, dynamics.states.time, "Time [s]");
    auto plot_combo = new ComboPlot();
    plot_combo->addData("Time [s]", dynamics.states.time);
    plot_combo->addData("Arrow position [m]", dynamics.states.pos_arrow);
    plot_combo->addData("Arrow velocity [m/s]", dynamics.states.vel_arrow);
    plot_combo->addData("Arrow acceleration [m/s²]", dynamics.states.acc_arrow);
    plot_combo->setCombination(0, 1);

    auto tabs = new QTabWidget();
    tabs->addTab(plot_shapes, "Shape");
    tabs->addTab(plot_stress, "Stress");
    tabs->addTab(plot_energy, "Energy");
    tabs->addTab(plot_combo, "Other");
    vbox->addWidget(tabs);

    auto slider = new Slider(dynamics.states.time, "Time [s]:");
    QObject::connect(slider, &Slider::valueChanged, plot_shapes, &ShapePlot::setStateIndex);
    QObject::connect(slider, &Slider::valueChanged, plot_stress, &StressPlot::setStateIndex);
    QObject::connect(slider, &Slider::valueChanged, plot_energy, &EnergyPlot::setStateIndex);
    emit slider->valueChanged(0);
    vbox->addWidget(slider);
}

OutputDialog::OutputDialog(QWidget* parent, const OutputData& output)
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

// Keep dialog from closing on enter
// http://stackoverflow.com/questions/15845487/how-do-i-prevent-the-enter-key-from-closing-my-qdialog-qt-4-8-1
void OutputDialog::keyPressEvent(QKeyEvent *evt)
{
    if(evt->key() == Qt::Key_Enter || evt->key() == Qt::Key_Return)
        return;

    QDialog::keyPressEvent(evt);
}
