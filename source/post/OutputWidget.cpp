#include "OutputWidget.hpp"
#include "NumberGrid.hpp"
#include "ShapePlot.hpp"
#include "StressPlot.hpp"
#include "CurvaturePlot.hpp"
#include "EnergyPlot.hpp"
#include "ComboPlot.hpp"
#include "Slider.hpp"
#include "gui/utils/UserSettings.hpp"

OutputWidget::OutputWidget(const OutputData& data)
    : data(data),
      button_statics(new QPushButton("Statics")),
      button_dynamics(new QPushButton("Dynamics"))
{
    bool enable_statics = !data.statics.states.time.empty();
    bool enable_dynamics = !data.dynamics.states.time.empty();

    auto stack = new QStackedLayout();
    if(enable_statics) {
        stack->addWidget(new StaticOutputWidget(data));
    }
    if(enable_dynamics) {
        stack->addWidget(new DynamicOutputWidget(data));
    }

    button_statics->setCheckable(true);
    button_statics->setEnabled(enable_statics);
    button_statics->setAutoExclusive(true);
    QObject::connect(button_statics, &QPushButton::toggled, [=](bool checked) {
        if(checked) {
            stack->setCurrentIndex(0);
        }
    });

    button_dynamics->setCheckable(true);
    button_dynamics->setEnabled(enable_dynamics);
    button_dynamics->setAutoExclusive(true);
    QObject::connect(button_dynamics, &QPushButton::toggled, [=](bool checked) {
        if(checked) {
            stack->setCurrentIndex(1);
        }
    });

    auto btbox = new QDialogButtonBox();
    btbox->addButton(button_statics, QDialogButtonBox::ActionRole);
    btbox->addButton(button_dynamics, QDialogButtonBox::ActionRole);
    QObject::connect(btbox, &QDialogButtonBox::rejected, this, &QDialog::close);

    auto vbox = new QVBoxLayout();
    vbox->addLayout(stack, 1);
    vbox->addWidget(btbox);
    this->setLayout(vbox);
    this->setAutoFillBackground(true);

    UserSettings settings;
    int mode = settings.value("OutputWidget/selectedMode", int(enable_dynamics)).toInt();

    bool statics_checked = (mode == 0 || !enable_dynamics);
    button_statics->setChecked(statics_checked);
    button_dynamics->setChecked(!statics_checked);
}

OutputWidget::~OutputWidget() {
    UserSettings settings;
    if(button_statics->isChecked()) {
        settings.setValue("OutputWidget/selectedMode", 0);
    }
    else if(button_dynamics->isChecked()) {
        settings.setValue("OutputWidget/selectedMode", 1);
    }
}


const OutputData& OutputWidget::getData() {
    return data;
}

StaticOutputWidget::StaticOutputWidget(const OutputData& data)
    : tabs(new QTabWidget())
{
    auto numbers = new NumberGrid();
    numbers->addColumn();
    numbers->addGroup("Performance");
    numbers->addValue("Final draw force", data.statics.final_draw_force, Quantities::force);
    numbers->addValue("Drawing work", data.statics.drawing_work, Quantities::energy);
    numbers->addValue("Energy storage factor", data.statics.energy_storage_factor, Quantities::ratio);
    numbers->addGroup("Properties");
    numbers->addValue("Limb mass", data.setup.limb_mass, Quantities::mass);
    numbers->addValue("String mass", data.setup.string_mass, Quantities::mass);
    numbers->addValue("String length", data.setup.string_length, Quantities::mass);

    numbers->addColumn();
    numbers->addGroup("Minimum stress by layer");
    for(size_t i = 0; i < data.statics.min_stress_index.size(); ++i) {
        numbers->addValue(QString::fromStdString(data.setup.limb_properties.layers[i].name), data.statics.min_stress_value[i], Quantities::stress);
    }
    numbers->addGroup("Maximum stress by layer");
    for(size_t i = 0; i < data.statics.max_stress_index.size(); ++i) {
        numbers->addValue(QString::fromStdString(data.setup.limb_properties.layers[i].name), data.statics.max_stress_value[i], Quantities::stress);
    }

    numbers->addColumn();
    numbers->addGroup("Maximum absolute forces");
    numbers->addValue("Draw force", data.statics.states.draw_force[data.statics.max_draw_force_index], Quantities::force);
    numbers->addValue("Grip force", data.statics.states.grip_force[data.statics.max_grip_force_index], Quantities::force);
    numbers->addValue("String force (total)", data.statics.states.string_force[data.statics.max_string_force_index], Quantities::force);
    numbers->addValue("String force (strand)", data.statics.states.strand_force[data.statics.max_string_force_index],  Quantities::force);

    auto plot_shapes = new ShapePlot(data.setup.limb_properties, data.statics.states, 4);
    auto plot_stress = new StressPlot(data.setup.limb_properties, data.statics.states);
    auto plot_curvature = new CurvaturePlot(data.setup.limb_properties, data.statics.states);
    auto plot_energy = new EnergyPlot(data.statics.states, data.statics.states.draw_length, "Draw length", Quantities::length, Quantities::energy);
    auto plot_combo = new ComboPlot();
    plot_combo->addData("Draw length", data.statics.states.draw_length, Quantities::length);
    plot_combo->addData("Draw force", data.statics.states.draw_force, Quantities::force);
    plot_combo->addData("String force (total)", data.statics.states.string_force, Quantities::force);
    plot_combo->addData("String force (strand)", data.statics.states.strand_force, Quantities::force);
    plot_combo->addData("Grip force", data.statics.states.grip_force, Quantities::force);
    plot_combo->addData("Pot. energy limbs", data.statics.states.e_pot_limbs, Quantities::energy);
    plot_combo->addData("Pot. energy string", data.statics.states.e_pot_string, Quantities::energy);
    plot_combo->setCombination(0, 1);

    tabs->addTab(numbers, "Characteristics");
    tabs->addTab(plot_shapes, "Shape");
    tabs->addTab(plot_stress, "Stress");
    tabs->addTab(plot_curvature, "Curvature");
    tabs->addTab(plot_energy, "Energy");
    tabs->addTab(plot_combo, "Other Plots");

    auto slider = new Slider(data.statics.states.draw_length, "Draw length", Quantities::length);
    slider->addJumpAction("Max. draw force", data.statics.max_draw_force_index);
    slider->addJumpAction("Max. grip force", data.statics.max_grip_force_index);
    slider->addJumpAction("Max. string force", data.statics.max_string_force_index);
    for (size_t i = 0; i < data.statics.max_stress_index.size(); ++i) {
        slider->addJumpAction(QString::fromStdString("Max. stress for layer:" + data.setup.limb_properties.layers[i].name), data.statics.max_stress_index[i].first);
    }

    QObject::connect(slider, &Slider::indexChanged, plot_shapes, &ShapePlot::setStateIndex);
    QObject::connect(slider, &Slider::indexChanged, plot_stress, &StressPlot::setStateIndex);
    QObject::connect(slider, &Slider::indexChanged, plot_curvature, &CurvaturePlot::setStateIndex);
    QObject::connect(slider, &Slider::indexChanged, plot_energy, &EnergyPlot::setStateIndex);
    emit slider->indexChanged(0);

    auto vbox = new QVBoxLayout();
    this->setLayout(vbox);
    vbox->setMargin(0);
    vbox->addWidget(tabs);
    vbox->addWidget(slider);

    UserSettings settings;
    tabs->setCurrentIndex(settings.value("StaticOutputWidget/selectedTab", tabs->currentIndex()).toInt());
}

StaticOutputWidget::~StaticOutputWidget() {
    UserSettings settings;
    settings.setValue("StaticOutputWidget/selectedTab", tabs->currentIndex());
}


DynamicOutputWidget::DynamicOutputWidget(const OutputData& data)
    : tabs(new QTabWidget())
{
    auto numbers = new NumberGrid();
    numbers->addColumn();
    numbers->addGroup("Performance");
    numbers->addValue("Final arrow velocity", data.dynamics.final_vel_arrow, Quantities::velocity);
    numbers->addValue("Degree of efficiency", data.dynamics.efficiency, Quantities::ratio);
    numbers->addGroup("Energy at arrow departure");
    numbers->addValue("Kinetic energy arrow", data.dynamics.final_e_kin_arrow, Quantities::energy);
    numbers->addValue("Kinetic energy limbs", data.dynamics.final_e_kin_limbs, Quantities::energy);
    numbers->addValue("Kinetic energy string", data.dynamics.final_e_kin_string, Quantities::energy);
    numbers->addColumn();
    numbers->addGroup("Maximum absolute stresses");
    for (size_t i = 0; i < data.dynamics.max_stress_value.size(); ++i) {
        numbers->addValue(QString::fromStdString(data.setup.limb_properties.layers[i].name), data.dynamics.max_stress_value[i], Quantities::stress);
    }
    numbers->addGroup("Maximum absolute forces");
    numbers->addValue("String force (total)", data.dynamics.states.string_force[data.dynamics.max_string_force_index], Quantities::force);
    numbers->addValue("String force (strand)", data.dynamics.states.strand_force[data.dynamics.max_string_force_index], Quantities::force);
    numbers->addValue("Grip force", data.dynamics.states.grip_force[data.dynamics.max_grip_force_index], Quantities::force);

    auto plot_shapes = new ShapePlot(data.setup.limb_properties, data.dynamics.states, 0);
    auto plot_stress = new StressPlot(data.setup.limb_properties, data.dynamics.states);
    auto plot_curvature = new CurvaturePlot(data.setup.limb_properties, data.dynamics.states);
    auto plot_energy = new EnergyPlot(data.dynamics.states, data.dynamics.states.time, "Time", Quantities::time, Quantities::energy);
    auto plot_combo = new ComboPlot();
    plot_combo->addData("Time", data.dynamics.states.time, Quantities::time);
    plot_combo->addData("Arrow position", data.dynamics.states.pos_arrow, Quantities::position);
    plot_combo->addData("Arrow velocity", data.dynamics.states.vel_arrow, Quantities::velocity);
    plot_combo->addData("Arrow acceleration", data.dynamics.states.acc_arrow, Quantities::acceleration);
    plot_combo->addData("String force (total)", data.dynamics.states.string_force, Quantities::force);
    plot_combo->addData("String force (strand)", data.dynamics.states.strand_force, Quantities::force);
    plot_combo->addData("Grip force", data.dynamics.states.grip_force, Quantities::force);
    plot_combo->addData("Pot. energy limbs", data.dynamics.states.e_pot_limbs, Quantities::energy);
    plot_combo->addData("Kin. energy limbs", data.dynamics.states.e_kin_limbs, Quantities::energy);
    plot_combo->addData("Pot. energy string", data.dynamics.states.e_pot_string, Quantities::energy);
    plot_combo->addData("Kin. energy string", data.dynamics.states.e_kin_string, Quantities::energy);
    plot_combo->addData("Kin. energy arrow", data.dynamics.states.e_kin_arrow, Quantities::energy);
    plot_combo->setCombination(0, 1);

    tabs->addTab(numbers, "Characteristics");
    tabs->addTab(plot_shapes, "Shape");
    tabs->addTab(plot_stress, "Stress");
    tabs->addTab(plot_curvature, "Curvature");
    tabs->addTab(plot_energy, "Energy");
    tabs->addTab(plot_combo, "Other Plots");

    auto slider = new Slider(data.dynamics.states.time, "Time", Quantities::time);
    slider->addJumpAction("Arrow departure", data.dynamics.arrow_departure_index);
    slider->addJumpAction("Max. grip force", data.dynamics.max_grip_force_index);
    slider->addJumpAction("Max. string force", data.dynamics.max_string_force_index);
    for (size_t i = 0; i < data.dynamics.max_stress_index.size(); ++i) {
        slider->addJumpAction(QString::fromStdString("Max. stress for layer: " + data.setup.limb_properties.layers[i].name), data.dynamics.max_stress_index[i].first);
    }

    QObject::connect(slider, &Slider::indexChanged, plot_shapes, &ShapePlot::setStateIndex);
    QObject::connect(slider, &Slider::indexChanged, plot_stress, &StressPlot::setStateIndex);
    QObject::connect(slider, &Slider::indexChanged, plot_curvature, &CurvaturePlot::setStateIndex);
    QObject::connect(slider, &Slider::indexChanged, plot_energy, &EnergyPlot::setStateIndex);
    emit slider->indexChanged(0);

    auto vbox = new QVBoxLayout();
    this->setLayout(vbox);
    vbox->setMargin(0);
    vbox->addWidget(tabs);
    vbox->addWidget(slider);

    UserSettings settings;
    tabs->setCurrentIndex(settings.value("DynamicOutputWidget/selectedTab", tabs->currentIndex()).toInt());
}

DynamicOutputWidget::~DynamicOutputWidget() {
    UserSettings settings;
    settings.setValue("DynamicOutputWidget/selectedTab", tabs->currentIndex());
}
