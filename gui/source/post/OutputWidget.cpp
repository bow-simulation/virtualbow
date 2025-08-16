#include "solver/BowResult.hpp"
#include "pre/models/units/UnitSystem.hpp"
#include "OutputWidget.hpp"
#include "NumberGrid.hpp"
#include "ShapePlot.hpp"
#include "StressPlot.hpp"
#include "CurvaturePlot.hpp"
#include "EnergyPlot.hpp"
#include "ComboPlot.hpp"
#include "Slider.hpp"
#include "pre/utils/UserSettings.hpp"
#include "pre/widgets/ScrollArea.hpp"

OutputWidget::OutputWidget(const BowResult& data)
    : data(data),
      button_statics(new QPushButton("Statics")),
      button_dynamics(new QPushButton("Dynamics"))
{
    bool enable_statics = data.statics.has_value();
    bool enable_dynamics = data.dynamics.has_value();

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

const BowResult& OutputWidget::getData() {
    return data;
}

StaticOutputWidget::StaticOutputWidget(const BowResult& data)
    : tabs(new QTabWidget())
{
    auto numbers = new NumberGrid();
    numbers->addColumn();
    numbers->addGroup("Performance");
    numbers->addValue("Final draw force", data.statics->final_draw_force, Quantities::force);
    numbers->addValue("Drawing work", data.statics->final_drawing_work, Quantities::energy);
    numbers->addValue("Energy storage factor", data.statics->storage_factor, Quantities::ratio);
    numbers->addGroup("Properties");
    numbers->addValue("Limb mass", data.common.limb_mass, Quantities::mass);
    numbers->addValue("String mass", data.common.string_mass, Quantities::mass);
    numbers->addValue("String length", data.common.string_length, Quantities::length);

    numbers->addColumn();
    numbers->addGroup("Maximum stresses");
    numbers->addHeaders({"Tension", "Compression"});
    for(size_t i = 0; i < data.common.layers.size(); ++i) {
        double min = std::get<0>(data.statics->min_layer_stresses.at(i));
        double max = std::get<0>(data.statics->max_layer_stresses.at(i));
        double tension = (max > 0.0) ? max : 0.0;         // There is only tension if the maximum stress is positive
        double compression = (min < 0.0) ? -min : 0.0;    // There is only compression if the minimum stress is negative
        numbers->addValues(QString::fromStdString(data.common.layers.at(i).name), {tension, compression}, {&Quantities::stress, &Quantities::stress});
    }

    numbers->addGroup("Maximum strains");
    numbers->addHeaders({"Tension", "Compression"});
    for(size_t i = 0; i < data.common.layers.size(); ++i) {
        double min = std::get<0>(data.statics->min_layer_strains.at(i));
        double max = std::get<0>(data.statics->max_layer_strains.at(i));
        double tension = (max > 0.0) ? max : 0.0;         // There is only tension if the maximum stress is positive
        double compression = (min < 0.0) ? -min : 0.0;    // There is only compression if the minimum stress is negative
        numbers->addValues(QString::fromStdString(data.common.layers.at(i).name), {tension, compression}, {&Quantities::strain, &Quantities::strain}, 4);
    }

    numbers->addColumn();
    numbers->addGroup("Maximum absolute forces");
    numbers->addValue("Draw force", std::get<0>(data.statics->max_draw_force), Quantities::force);
    numbers->addValue("Grip force", std::get<0>(data.statics->max_grip_force), Quantities::force);
    numbers->addValue("String force (total)", std::get<0>(data.statics->max_string_force), Quantities::force);
    numbers->addValue("String force (strand)", std::get<0>(data.statics->max_strand_force),  Quantities::force);

    auto plot_shapes = new ShapePlot(data.common, data.statics->states, 4);
    auto plot_stress = new StressPlot(data.common, data.statics->states);
    auto plot_curvature = new CurvaturePlot(data.common, data.statics->states);
    auto plot_energy = new EnergyPlot(data.statics->states, data.statics->states.draw_length, "Draw length", Quantities::length, Quantities::energy);
    auto plot_combo = new ComboPlot();
    plot_combo->addData("Draw length", data.statics->states.draw_length, Quantities::length);
    plot_combo->addData("Draw force", data.statics->states.draw_force, Quantities::force);
    plot_combo->addData("String force (total)", data.statics->states.string_force, Quantities::force);
    plot_combo->addData("String force (strand)", data.statics->states.strand_force, Quantities::force);
    plot_combo->addData("Grip force", data.statics->states.grip_force, Quantities::force);
    plot_combo->addData("Pot. energy limbs", data.statics->states.elastic_energy_limbs, Quantities::energy);
    plot_combo->addData("Pot. energy string", data.statics->states.elastic_energy_string, Quantities::energy);
    plot_combo->setCombination(0, 1);

    tabs->addTab(scrollArea(numbers), "Characteristics");
    tabs->addTab(plot_shapes, "Shape");
    tabs->addTab(plot_stress, "Stress");
    tabs->addTab(plot_curvature, "Curvature");
    tabs->addTab(plot_energy, "Energy");
    tabs->addTab(plot_combo, "Other Plots");

    auto slider = new Slider(data.statics->states.draw_length, "Draw length", Quantities::length);
    slider->addJumpAction("Max. draw force", std::get<1>(data.statics->max_draw_force));
    slider->addJumpAction("Max. grip force", std::get<1>(data.statics->max_grip_force));
    slider->addJumpAction("Max. string force", std::get<1>(data.statics->max_string_force));
    for(size_t i = 0; i < data.statics->max_layer_stresses.size(); ++i) {
        slider->addJumpAction(QString::fromStdString("Max. stress for layer: TODO"), std::get<0>(data.statics->max_layer_stresses[i]));
    }

    QObject::connect(slider, &Slider::indexChanged, plot_shapes, &ShapePlot::setStateIndex);
    QObject::connect(slider, &Slider::indexChanged, plot_stress, &StressPlot::setStateIndex);
    QObject::connect(slider, &Slider::indexChanged, plot_curvature, &CurvaturePlot::setStateIndex);
    QObject::connect(slider, &Slider::indexChanged, plot_energy, &EnergyPlot::setStateIndex);
    emit slider->indexChanged(0);

    auto vbox = new QVBoxLayout();
    this->setLayout(vbox);
    vbox->setContentsMargins(0, 0, 0, 0);
    vbox->addWidget(tabs);
    vbox->addWidget(slider);

    UserSettings settings;
    tabs->setCurrentIndex(settings.value("StaticOutputWidget/selectedTab", tabs->currentIndex()).toInt());
}

StaticOutputWidget::~StaticOutputWidget() {
    UserSettings settings;
    settings.setValue("StaticOutputWidget/selectedTab", tabs->currentIndex());
}


DynamicOutputWidget::DynamicOutputWidget(const BowResult& data)
    : tabs(new QTabWidget())
{
    auto numbers = new NumberGrid();
    if(data.dynamics->arrow_departure.has_value()) {
        auto& departure = *data.dynamics->arrow_departure;

        numbers->addColumn();
        numbers->addGroup("Performance");
        numbers->addValue("Final arrow velocity", departure.arrow_vel, Quantities::velocity);
        numbers->addValue("Degree of efficiency", departure.energy_efficiency, Quantities::ratio);

        double total_energy = departure.kinetic_energy_arrow + departure.kinetic_energy_limbs + departure.elastic_energy_limbs + departure.damping_energy_limbs
                              + departure.kinetic_energy_string + departure.elastic_energy_string + departure.damping_energy_string;

        numbers->addGroup("Energies at arrow departure");
        numbers->addHeaders({"Absolute", "Relative"});
        numbers->addValues("Arrow (kinetic)", {departure.kinetic_energy_arrow, departure.kinetic_energy_arrow/total_energy}, {&Quantities::energy, &Quantities::ratio});
        numbers->addValues("Limbs (kinetic)", {departure.kinetic_energy_limbs, departure.kinetic_energy_limbs/total_energy}, {&Quantities::energy, &Quantities::ratio});
        numbers->addValues("Limbs (elastic)", {departure.elastic_energy_limbs, departure.elastic_energy_limbs/total_energy}, {&Quantities::energy, &Quantities::ratio});
        numbers->addValues("Limbs (damping)", {departure.damping_energy_limbs, departure.damping_energy_limbs/total_energy}, {&Quantities::energy, &Quantities::ratio});
        numbers->addValues("String (kinetic)", {departure.kinetic_energy_string, departure.kinetic_energy_string/total_energy}, {&Quantities::energy, &Quantities::ratio});
        numbers->addValues("String (elastic)", {departure.elastic_energy_string, departure.elastic_energy_string/total_energy}, {&Quantities::energy, &Quantities::ratio});
        numbers->addValues("String (damping)", {departure.damping_energy_string, departure.damping_energy_string/total_energy}, {&Quantities::energy, &Quantities::ratio});
    }

    numbers->addColumn();
    numbers->addGroup("Maximum stresses");
    numbers->addHeaders({"Tension", "Compression"});
    for(size_t i = 0; i < data.common.layers.size(); ++i) {
        double min_stress = std::get<0>(data.dynamics->min_layer_stresses.at(i));
        double max_stress = std::get<0>(data.dynamics->max_layer_stresses.at(i));
        double tension = (max_stress > 0.0) ? max_stress : 0.0;         // There is only tension if the maximum stress is positive
        double compression = (min_stress < 0.0) ? -min_stress : 0.0;    // There is only compression if the minimum stress is negative
        numbers->addValues(QString::fromStdString(data.common.layers.at(i).name), {tension, compression}, {&Quantities::stress, &Quantities::stress});
    }

    numbers->addGroup("Maximum strains");
    numbers->addHeaders({"Tension", "Compression"});
    for(size_t i = 0; i < data.common.layers.size(); ++i) {
        double min = std::get<0>(data.dynamics->min_layer_strains.at(i));
        double max = std::get<0>(data.dynamics->max_layer_strains.at(i));
        double tension = (max > 0.0) ? max : 0.0;         // There is only tension if the maximum stress is positive
        double compression = (min < 0.0) ? -min : 0.0;    // There is only compression if the minimum stress is negative
        numbers->addValues(QString::fromStdString(data.common.layers.at(i).name), {tension, compression}, {&Quantities::strain, &Quantities::strain}, 4);
    }

    numbers->addColumn();
    numbers->addGroup("Maximum forces");
    numbers->addValue("Grip push force", std::get<0>(data.dynamics->max_grip_force), Quantities::force);
    numbers->addValue("Grip pull force", std::get<0>(data.dynamics->min_grip_force), Quantities::force);
    numbers->addValue("String force (total)", std::get<0>(data.dynamics->max_string_force), Quantities::force);
    numbers->addValue("String force (strand)", std::get<0>(data.dynamics->max_strand_force), Quantities::force);

    auto plot_shapes = new ShapePlot(data.common, data.dynamics->states, 0);
    auto plot_stress = new StressPlot(data.common, data.dynamics->states);
    auto plot_curvature = new CurvaturePlot(data.common, data.dynamics->states);
    auto plot_energy = new EnergyPlot(data.dynamics->states, data.dynamics->states.time, "Time", Quantities::time, Quantities::energy);
    auto plot_combo = new ComboPlot();
    plot_combo->addData("Time", data.dynamics->states.time, Quantities::time);
    plot_combo->addData("Arrow position", data.dynamics->states.arrow_pos, Quantities::position);
    plot_combo->addData("Arrow velocity", data.dynamics->states.arrow_vel, Quantities::velocity);
    plot_combo->addData("Arrow acceleration", data.dynamics->states.arrow_acc, Quantities::acceleration);
    plot_combo->addData("String force (total)", data.dynamics->states.string_force, Quantities::force);
    plot_combo->addData("String force (strand)", data.dynamics->states.strand_force, Quantities::force);
    plot_combo->addData("Grip force", data.dynamics->states.grip_force, Quantities::force);
    plot_combo->addData("Pot. energy limbs", data.dynamics->states.elastic_energy_limbs, Quantities::energy);
    plot_combo->addData("Kin. energy limbs", data.dynamics->states.kinetic_energy_limbs, Quantities::energy);
    plot_combo->addData("Pot. energy string", data.dynamics->states.elastic_energy_string, Quantities::energy);
    plot_combo->addData("Kin. energy string", data.dynamics->states.kinetic_energy_string, Quantities::energy);
    plot_combo->addData("Kin. energy arrow", data.dynamics->states.kinetic_energy_arrow, Quantities::energy);
    plot_combo->setCombination(0, 1);

    tabs->addTab(scrollArea(numbers), "Characteristics");
    tabs->addTab(plot_shapes, "Shape");
    tabs->addTab(plot_stress, "Stress");
    tabs->addTab(plot_curvature, "Curvature");
    tabs->addTab(plot_energy, "Energy");
    tabs->addTab(plot_combo, "Other Plots");

    auto slider = new Slider(data.dynamics->states.time, "Time", Quantities::time);
    if(data.dynamics->arrow_departure.has_value()) {
        slider->addJumpAction("Arrow departure", data.dynamics->arrow_departure->state_idx);
    }
    slider->addJumpAction("Max. grip force (push)", std::get<1>(data.dynamics->max_grip_force));
    slider->addJumpAction("Min. grip force (pull)", std::get<1>(data.dynamics->min_grip_force));
    slider->addJumpAction("Max. string force", std::get<1>(data.dynamics->max_string_force));
    for (size_t i = 0; i < data.dynamics->max_layer_stresses.size(); ++i) {
        slider->addJumpAction(QString::fromStdString("Max. stress for layer: " + data.common.layers[i].name), std::get<0>(data.dynamics->max_layer_stresses[i]));
    }

    QObject::connect(slider, &Slider::indexChanged, plot_shapes, &ShapePlot::setStateIndex);
    QObject::connect(slider, &Slider::indexChanged, plot_stress, &StressPlot::setStateIndex);
    QObject::connect(slider, &Slider::indexChanged, plot_curvature, &CurvaturePlot::setStateIndex);
    QObject::connect(slider, &Slider::indexChanged, plot_energy, &EnergyPlot::setStateIndex);
    emit slider->indexChanged(0);

    auto vbox = new QVBoxLayout();
    this->setLayout(vbox);
    vbox->setContentsMargins(0, 0, 0, 0);
    vbox->addWidget(tabs);
    vbox->addWidget(slider);

    UserSettings settings;
    tabs->setCurrentIndex(settings.value("DynamicOutputWidget/selectedTab", tabs->currentIndex()).toInt());
}

DynamicOutputWidget::~DynamicOutputWidget() {
    UserSettings settings;
    settings.setValue("DynamicOutputWidget/selectedTab", tabs->currentIndex());
}
