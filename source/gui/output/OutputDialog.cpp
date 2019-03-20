#include "OutputDialog.hpp"
#include "OutputGrid.hpp"
#include "ShapePlot.hpp"
#include "StressPlot.hpp"
#include "CurvaturePlot.hpp"
#include "EnergyPlot.hpp"
#include "ComboPlot.hpp"
#include "Slider.hpp"

StaticOutput::StaticOutput(const InputData& input, const LimbProperties& limb, const StaticData& statics)
{
    auto vbox = new QVBoxLayout();
    this->setLayout(vbox);
    vbox->setMargin(0);

    auto plot_shapes = new ShapePlot(limb, statics.states, true);
    auto plot_stress = new StressPlot(input, limb, statics.states);
    auto plot_curvature = new CurvaturePlot(input, limb, statics.states);
    auto plot_energy = new EnergyPlot(statics.states, statics.states.draw_length, "Draw length [m]");
    auto plot_combo = new ComboPlot();
    plot_combo->addData("Draw length [m]", statics.states.draw_length);
    plot_combo->addData("Draw force [N]", statics.states.draw_force);
    plot_combo->addData("String force (total) [N]", statics.states.string_force);
    plot_combo->addData("String force (strand) [N]", statics.states.strand_force);
    plot_combo->addData("Grip force [N]", statics.states.grip_force);
    plot_combo->addData("Pot. energy limbs [J]", statics.states.e_pot_limbs);
    plot_combo->addData("Pot. energy string [J]", statics.states.e_pot_string);
    plot_combo->setCombination(0, 1);

    auto grid = new OutputGrid();
    grid->add(0, 0, "String length [m]", statics.string_length);
    grid->add(0, 1, "String length [\"]", statics.string_length * 100 / 2.54);
    grid->add(1, 0, "Final draw force [N]", statics.final_draw_force);
    grid->add(1, 1, "Final draw force [#]", statics.final_draw_force * 0.224809);
    grid->add(2, 0, "Drawing work [J]", statics.drawing_work);
    grid->add(3, 0, "Storage ratio", statics.storage_ratio);

    auto tabs = new QTabWidget();
    vbox->addWidget(tabs);
    tabs->addTab(plot_shapes, "Shape");
    tabs->addTab(plot_stress, "Stress");
    tabs->addTab(plot_curvature, "Curvature");
    tabs->addTab(plot_energy, "Energy");
    tabs->addTab(plot_combo, "Other Plots");
    tabs->addTab(grid, "Special Values");

    auto slider = new Slider(statics.states.draw_length, "Draw length [m]:");
    QObject::connect(slider, &Slider::valueChanged, plot_shapes, &ShapePlot::setStateIndex);
    QObject::connect(slider, &Slider::valueChanged, plot_stress, &StressPlot::setStateIndex);
    QObject::connect(slider, &Slider::valueChanged, plot_curvature, &CurvaturePlot::setStateIndex);
    QObject::connect(slider, &Slider::valueChanged, plot_energy, &EnergyPlot::setStateIndex);
    emit slider->valueChanged(0);
    vbox->addWidget(slider);
}

DynamicOutput::DynamicOutput(const InputData& input, const LimbProperties& limb, const DynamicData& dynamics)
{
    auto vbox = new QVBoxLayout();
    this->setLayout(vbox);
    vbox->setMargin(0);

    auto plot_shapes = new ShapePlot(limb, dynamics.states, false);
    auto plot_stress = new StressPlot(input, limb, dynamics.states);
    auto plot_curvature = new CurvaturePlot(input, limb, dynamics.states);
    auto plot_energy = new EnergyPlot(dynamics.states, dynamics.states.time, "Time [s]");
    auto plot_combo = new ComboPlot();
    plot_combo->addData("Time [s]", dynamics.states.time);
    plot_combo->addData("Arrow position [m]", dynamics.states.pos_arrow);
    plot_combo->addData("Arrow velocity [m/s]", dynamics.states.vel_arrow);
    plot_combo->addData("Arrow acceleration [m/s²]", dynamics.states.acc_arrow);
    plot_combo->addData("String force (total) [N]", dynamics.states.string_force);
    plot_combo->addData("String force (strand) [N]", dynamics.states.strand_force);
    plot_combo->addData("Grip force [N]", dynamics.states.grip_force);
    plot_combo->addData("Pot. energy limbs [J]", dynamics.states.e_pot_limbs);
    plot_combo->addData("Kin. energy limbs [J]", dynamics.states.e_kin_limbs);
    plot_combo->addData("Pot. energy string [J]", dynamics.states.e_pot_string);
    plot_combo->addData("Kin. energy string [J]", dynamics.states.e_kin_string);
    plot_combo->addData("Kin. energy arrow [J]", dynamics.states.e_kin_arrow);
    plot_combo->setCombination(0, 1);

    auto grid = new OutputGrid();
    grid->add(0, 0, "Final arrow velocity [m/s]", dynamics.final_arrow_velocity);
    grid->add(0, 1, "Final arrow velocity [f/s]", dynamics.final_arrow_velocity * 3.28084);
    grid->add(1, 0, "Maximum arrow height [m]", dynamics.final_arrow_velocity*dynamics.final_arrow_velocity/2/9.81);
    grid->add(2, 0, "Final arrow energy [J]", dynamics.final_arrow_energy);
    grid->add(3, 0, "Efficiency", dynamics.efficiency);

    auto tabs = new QTabWidget();
    vbox->addWidget(tabs);
    tabs->addTab(plot_shapes, "Shape");
    tabs->addTab(plot_stress, "Stress");
    tabs->addTab(plot_curvature, "Curvature");
    tabs->addTab(plot_energy, "Energy");
    tabs->addTab(plot_combo, "Other Plots");
    tabs->addTab(grid, "Special Values");

    auto slider = new Slider(dynamics.states.time, "Time [s]:");
    QObject::connect(slider, &Slider::valueChanged, plot_shapes, &ShapePlot::setStateIndex);
    QObject::connect(slider, &Slider::valueChanged, plot_stress, &StressPlot::setStateIndex);
    QObject::connect(slider, &Slider::valueChanged, plot_curvature, &CurvaturePlot::setStateIndex);
    QObject::connect(slider, &Slider::valueChanged, plot_energy, &EnergyPlot::setStateIndex);
    emit slider->valueChanged(0);
    vbox->addWidget(slider);
}

OutputDialog::OutputDialog(QWidget* parent, const InputData& input, const OutputData& output)
    : PersistentDialog(parent, "OutputDialog", {1000, 700})    // Magic numbers
{
    auto vbox = new QVBoxLayout();
    this->setLayout(vbox);
    this->setWindowTitle("Simulation Results");
    this->setWindowFlags(this->windowFlags() | Qt::WindowMaximizeButtonHint);

    bool enable_statics = !output.statics.states.time.empty();
    bool enable_dynamics = !output.dynamics.states.time.empty();

    auto stack = new QStackedLayout();
    vbox->addLayout(stack, 1);
    if(enable_statics)
        stack->addWidget(new StaticOutput(input, output.limb_properties, output.statics));
    if(enable_dynamics)
        stack->addWidget(new DynamicOutput(input, output.limb_properties, output.dynamics));

    auto bt_statics = new QPushButton("Statics");
    // bt_statics->setStyleSheet("background-color: rgb(249, 217, 111);");
    bt_statics->setIcon(QIcon(":/icons/show-statics"));
    bt_statics->setCheckable(true);
    bt_statics->setChecked(true);
    bt_statics->setEnabled(enable_statics);
    bt_statics->setAutoExclusive(true);

    auto bt_dynamics = new QPushButton("Dynamics");
    // bt_dynamics->setStyleSheet("background-color: rgb(170, 243, 117);");
    bt_dynamics->setIcon(QIcon(":/icons/show-dynamics"));
    bt_dynamics->setCheckable(true);
    bt_dynamics->setChecked(false);
    bt_dynamics->setEnabled(enable_dynamics);
    bt_dynamics->setAutoExclusive(true);

    auto btbox = new QDialogButtonBox();
    btbox->addButton(bt_statics, QDialogButtonBox::ResetRole);
    btbox->addButton(bt_dynamics, QDialogButtonBox::ResetRole);
    btbox->addButton(QDialogButtonBox::Close);
    vbox->addWidget(btbox);

    QObject::connect(bt_statics, &QPushButton::toggled, [=](bool checked){
        if(checked)
            stack->setCurrentIndex(0);
    });

    QObject::connect(bt_dynamics, &QPushButton::toggled, [=](bool checked){
        if(checked)
            stack->setCurrentIndex(1);
    });

    QObject::connect(btbox, &QDialogButtonBox::rejected, this, &QDialog::close);
}
