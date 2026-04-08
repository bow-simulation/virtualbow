#include "solver/BowResult.hpp"
#include "pre/models/units/UnitSystem.hpp"
#include "pre/widgets/DoubleOutput.hpp"
#include "pre/Language.hpp"
#include "OutputWidget.hpp"
#include "OutputGrid.hpp"
#include "ShapePlot.hpp"
#include "ArrowPlot.hpp"
#include "DrawForcePlot.hpp"
#include "StressPlot.hpp"
#include "CurvaturePlot.hpp"
#include "EnergyPlot.hpp"
#include "ComboPlot.hpp"
#include "Slider.hpp"
#include "pre/utils/UserSettings.hpp"
#include "pre/widgets/ScrollArea.hpp"

OutputWidget::OutputWidget(const BowResult& data)
    : data(data)
{
    button_statics = new QPushButton("Statics");
    button_statics->setToolTip(Tooltips::ButtonShowStatics);

    button_dynamics = new QPushButton("Dynamics");
    button_dynamics->setToolTip(Tooltips::ButtonShowDynamics);

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
    auto numbers = new OutputGrid();
    numbers->addColumn();
    numbers->addGroup("Performance");
    numbers->addValue("Final draw force", new DoubleOutput(data.statics->final_draw_force, Quantities::force, 2, Tooltips::OutputFinalDrawForce));
    numbers->addValue("Drawing work", new DoubleOutput(data.statics->final_drawing_work, Quantities::energy, 2, Tooltips::OutputDrawingWork));
    numbers->addValue("Energy storage factor", new DoubleOutput(data.statics->storage_factor, Quantities::ratio, 2, Tooltips::OutputEnergyStorageFactor));
    numbers->addGroup("Properties");
    numbers->addValue("Limb mass", new DoubleOutput(data.common.limb_mass, Quantities::mass, 2, Tooltips::OutputLimbMass));
    numbers->addValue("String mass", new DoubleOutput(data.common.string_mass, Quantities::mass, 2, Tooltips::OutputStringMass));
    numbers->addValue("String length", new DoubleOutput(data.common.string_length, Quantities::length, 2, Tooltips::OutputStringLength));
    numbers->addValue("Power stroke", new DoubleOutput(data.common.power_stroke, Quantities::length, 2, Tooltips::OutputPowerStroke));

    numbers->addColumn();
    numbers->addGroup("Maximum stresses");
    numbers->addHeaders({"Tension", "Compression"});
    for(size_t i = 0; i < data.common.layers.size(); ++i) {
        QString layer = QString::fromStdString(data.common.layers[i].name);
        double tension = std::get<0>(data.statics->max_stresses.max_layer_stress_tension.at(i));
        double compression = std::get<0>(data.statics->max_stresses.max_layer_stress_compression.at(i));

        double allowed_tension = data.common.layers.at(i).allowed_stresses.first;
        double allowed_compression = data.common.layers.at(i).allowed_stresses.second;
        double maximum_tension = data.common.layers.at(i).maximum_stresses.first;
        double maximum_compression = data.common.layers.at(i).maximum_stresses.second;

        numbers->addValues(layer, {
            new DoubleOutput(tension, allowed_tension, maximum_tension, Quantities::stress, 2, Tooltips::OutputMaxTensileStress.arg(layer)),
            new DoubleOutput(compression, allowed_compression, maximum_compression, Quantities::stress, 2, Tooltips::OutputMaxCompressiveStress.arg(layer))
        });
    }

    numbers->addGroup("Maximum strains");
    numbers->addHeaders({"Tension", "Compression"});
    for(size_t i = 0; i < data.common.layers.size(); ++i) {
        QString layer = QString::fromStdString(data.common.layers[i].name);
        double tension = std::get<0>(data.statics->max_stresses.max_layer_strain_tension.at(i));
        double compression = std::get<0>(data.statics->max_stresses.max_layer_strain_compression.at(i));

        double allowed_tension = data.common.layers.at(i).allowed_strains.first;
        double allowed_compression = data.common.layers.at(i).allowed_strains.second;
        double maximum_tension = data.common.layers.at(i).maximum_strains.first;
        double maximum_compression = data.common.layers.at(i).maximum_strains.second;

        numbers->addValues(layer, {
            new DoubleOutput(tension, allowed_tension, maximum_tension, Quantities::strain, 4, Tooltips::OutputMaxTensileStrain.arg(layer)),
            new DoubleOutput(compression, allowed_compression, maximum_compression, Quantities::strain, 4, Tooltips::OutputMaxCompressiveStrain.arg(layer))
        });
    }

    numbers->addColumn();
    numbers->addGroup("Maximum absolute forces");
    numbers->addValue("Draw force", new DoubleOutput(std::get<0>(data.statics->max_forces.max_draw_force), Quantities::force, 2, Tooltips::OutputMaxDrawForce));
    numbers->addValue("Grip force", new DoubleOutput(std::get<0>(data.statics->max_forces.max_grip_force), Quantities::force, 2, Tooltips::OutputMaxGripPushForce));
    numbers->addValue("String force (total)", new DoubleOutput(std::get<0>(data.statics->max_forces.max_string_force), Quantities::force, 2, Tooltips::OutputMaxStringForce));
    numbers->addValue("String force (strand)", new DoubleOutput(std::get<0>(data.statics->max_forces.max_strand_force),  Quantities::force, 2, Tooltips::OutputMaxStrandForce));

    auto plot_shapes = new ShapePlot(data.common, data.statics->states, 4);
    auto plot_draw = new DrawForcePlot(data.common, data.statics->states);
    auto plot_stress = new StressPlot(data.common, data.statics->states);
    auto plot_curvature = new CurvaturePlot(data.common, data.statics->states);
    auto plot_energy = new EnergyPlot(data.statics->states, data.statics->states.draw_length, "Draw length", Quantities::length, Quantities::energy);
    auto plot_combo = new ComboPlot();
    plot_combo->addData("Power stroke", data.statics->states.power_stroke, Quantities::length);
    plot_combo->addData("Draw length", data.statics->states.draw_length, Quantities::length);
    plot_combo->addData("Draw force", data.statics->states.draw_force, Quantities::force);
    plot_combo->addData("Draw stiffness", data.statics->states.draw_stiffness, Quantities::stiffness);
    plot_combo->addData("String force (total)", data.statics->states.string_force, Quantities::force);
    plot_combo->addData("String force (strand)", data.statics->states.strand_force, Quantities::force);
    plot_combo->addData("String length", data.statics->states.string_length, Quantities::length);
    plot_combo->addData("String tip angle", data.statics->states.string_tip_angle, Quantities::angle);
    plot_combo->addData("String center angle", data.statics->states.string_center_angle, Quantities::angle);
    plot_combo->addData("Grip force", data.statics->states.grip_force, Quantities::force);
    plot_combo->addData("Elastic energy limbs", data.statics->states.elastic_energy_limbs, Quantities::energy);
    plot_combo->addData("Elastic energy string", data.statics->states.elastic_energy_string, Quantities::energy);
    plot_combo->setCombination(1, 2);

    tabs->addTab(scrollArea(numbers), "Characteristics");
    tabs->addTab(plot_shapes, "Shape");
    tabs->addTab(plot_draw, "Draw");
    tabs->addTab(plot_stress, "Stress");
    tabs->addTab(plot_curvature, "Curvature");
    tabs->addTab(plot_energy, "Energy");
    tabs->addTab(plot_combo, "Other Plots");

    auto slider = new Slider(data.statics->states.draw_length, "Draw length", Quantities::length);
    slider->addJumpAction("Max. draw force", std::get<1>(data.statics->max_forces.max_draw_force));
    slider->addJumpAction("Max. grip force", std::get<1>(data.statics->max_forces.max_grip_force));
    slider->addJumpAction("Max. string force", std::get<1>(data.statics->max_forces.max_string_force));
    for(size_t i = 0; i < data.common.layers.size(); ++i) {
        slider->addJumpAction(QString::fromStdString("Max. tension for layer: " + data.common.layers[i].name), std::get<0>(data.statics->max_stresses.max_layer_stress_tension[i]));
        slider->addJumpAction(QString::fromStdString("Max. compression for layer: " + data.common.layers[i].name), std::get<0>(data.statics->max_stresses.max_layer_stress_compression[i]));
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
    auto numbers = new OutputGrid();
    if(data.dynamics->arrow_departure.has_value()) {
        auto& departure = *data.dynamics->arrow_departure;

        numbers->addColumn();
        numbers->addGroup("Performance");
        numbers->addValue("Arrow mass", new DoubleOutput(data.dynamics->arrow_mass, Quantities::mass, 2, Tooltips::OutputArrowMass));
        numbers->addValue("Final arrow velocity", new DoubleOutput(departure.arrow_vel, Quantities::velocity, 2, Tooltips::OutputFinalArrowVelocity));
        numbers->addValue("Final arrow energy", new DoubleOutput(departure.kinetic_energy_arrow, Quantities::energy, 2, Tooltips::OutputFinalArrowEnergy));
        numbers->addValue("Degree of efficiency", new DoubleOutput(departure.energy_efficiency, Quantities::ratio, 2, Tooltips::OutputEnergyEfficiency));

        double drawing_work = data.statics->final_drawing_work;
        double initial_energy_limbs = data.statics->states.elastic_energy_limbs[0];    // TODO: Make this a dedicated result?
        double initial_energy_string = data.statics->states.elastic_energy_string[0];    // TODO: Make this a dedicated result?

        numbers->addGroup("Efficiency losses");
        numbers->addHeaders({"Energy", "Efficiency"});
        numbers->addValues("Limbs (kinetic)", {
            new DoubleOutput(departure.kinetic_energy_limbs, Quantities::energy, 2, Tooltips::OutputEnergyLossLimbsKineticAbsolute),
            new DoubleOutput(departure.kinetic_energy_limbs/drawing_work, Quantities::ratio, 2, Tooltips::OutputEnergyLossLimbsKineticRelative)
        });
        numbers->addValues("Limbs (elastic)", {
            new DoubleOutput(departure.elastic_energy_limbs - initial_energy_limbs, Quantities::energy, 2, Tooltips::OutputEnergyLossLimbsElasticAbsolute),
            new DoubleOutput((departure.elastic_energy_limbs - initial_energy_limbs)/drawing_work, Quantities::ratio, 2, Tooltips::OutputEnergyLossLimbsElasticRelative)
        });
        numbers->addValues("Limbs (damping)", {
            new DoubleOutput(departure.damping_energy_limbs, Quantities::energy, 2, Tooltips::OutputEnergyLossLimbsDampingAbsolute),
            new DoubleOutput(departure.damping_energy_limbs/drawing_work, Quantities::ratio, 2, Tooltips::OutputEnergyLossLimbsDampingRelative)
        });
        numbers->addValues("String (kinetic)", {
            new DoubleOutput(departure.kinetic_energy_string, Quantities::energy, 2, Tooltips::OutputEnergyLossStringKineticAbsolute),
            new DoubleOutput(departure.kinetic_energy_string/drawing_work, Quantities::ratio, 2, Tooltips::OutputEnergyLossStringKineticRelative)
        });
        numbers->addValues("String (elastic)", {
            new DoubleOutput(departure.elastic_energy_string - initial_energy_string, Quantities::energy, 2, Tooltips::OutputEnergyLossStringElasticAbsolute),
            new DoubleOutput((departure.elastic_energy_string - initial_energy_string)/drawing_work, Quantities::ratio, 2, Tooltips::OutputEnergyLossStringElasticRelative)
        });
        numbers->addValues("String (damping)", {
            new DoubleOutput(departure.damping_energy_string, Quantities::energy, 2, Tooltips::OutputEnergyLossStringDampingAbsolute),
            new DoubleOutput(departure.damping_energy_string/drawing_work, Quantities::ratio, 2, Tooltips::OutputEnergyLossStringDampingRelative)
        });
    }

    numbers->addColumn();
    numbers->addGroup("Maximum stresses");
    numbers->addHeaders({"Tension", "Compression"});
    for(size_t i = 0; i < data.common.layers.size(); ++i) {
        QString layer = QString::fromStdString(data.common.layers[i].name);
        double tension = std::get<0>(data.dynamics->max_stresses.max_layer_stress_tension.at(i));
        double compression = std::get<0>(data.dynamics->max_stresses.max_layer_stress_compression.at(i));

        double allowed_tension = data.common.layers.at(i).allowed_stresses.first;
        double allowed_compression = data.common.layers.at(i).allowed_stresses.second;
        double maximum_tension = data.common.layers.at(i).maximum_stresses.first;
        double maximum_compression = data.common.layers.at(i).maximum_stresses.second;

        numbers->addValues(layer, {
            new DoubleOutput(tension, allowed_tension, maximum_tension, Quantities::stress, 2, Tooltips::OutputMaxTensileStress.arg(layer)),
            new DoubleOutput(compression, allowed_compression, maximum_compression, Quantities::stress, 2, Tooltips::OutputMaxCompressiveStress.arg(layer))
        });
    }

    numbers->addGroup("Maximum strains");
    numbers->addHeaders({"Tension", "Compression"});
    for(size_t i = 0; i < data.common.layers.size(); ++i) {
        QString layer = QString::fromStdString(data.common.layers[i].name);
        double tension = std::get<0>(data.dynamics->max_stresses.max_layer_strain_tension.at(i));
        double compression = std::get<0>(data.dynamics->max_stresses.max_layer_strain_compression.at(i));

        double allowed_tension = data.common.layers.at(i).allowed_strains.first;
        double allowed_compression = data.common.layers.at(i).allowed_strains.second;
        double maximum_tension = data.common.layers.at(i).maximum_strains.first;
        double maximum_compression = data.common.layers.at(i).maximum_strains.second;

        numbers->addValues(layer, {
            new DoubleOutput(tension, allowed_tension, maximum_tension, Quantities::strain, 4, Tooltips::OutputMaxTensileStrain.arg(layer)),
            new DoubleOutput(compression, allowed_compression, maximum_compression, Quantities::strain, 4, Tooltips::OutputMaxCompressiveStrain.arg(layer))
        });
    }

    numbers->addColumn();
    numbers->addGroup("Maximum forces");
    numbers->addValue("Grip push force", new DoubleOutput(std::get<0>(data.dynamics->max_forces.max_grip_force), Quantities::force, 2, Tooltips::OutputMaxGripPushForce));
    numbers->addValue("Grip pull force", new DoubleOutput(std::get<0>(data.dynamics->max_forces.min_grip_force), Quantities::force, 2, Tooltips::OutputMaxGripPullForce));
    numbers->addValue("String force (total)", new DoubleOutput(std::get<0>(data.dynamics->max_forces.max_string_force), Quantities::force, 2, Tooltips::OutputMaxStringForce));
    numbers->addValue("String force (strand)", new DoubleOutput(std::get<0>(data.dynamics->max_forces.max_strand_force), Quantities::force, 2, Tooltips::OutputMaxStrandForce));

    auto plot_shapes = new ShapePlot(data.common, data.dynamics->states, 0);
    auto plot_arrow = new ArrowPlot(data.common, data.dynamics->states);
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
    plot_combo->addData("String length", data.dynamics->states.string_length, Quantities::length);
    plot_combo->addData("String tip angle", data.dynamics->states.string_tip_angle, Quantities::angle);
    plot_combo->addData("String center angle", data.dynamics->states.string_center_angle, Quantities::angle);
    plot_combo->addData("Grip force", data.dynamics->states.grip_force, Quantities::force);
    plot_combo->addData("Elastic energy limbs", data.dynamics->states.elastic_energy_limbs, Quantities::energy);
    plot_combo->addData("Kinetic energy limbs", data.dynamics->states.kinetic_energy_limbs, Quantities::energy);
    plot_combo->addData("Elastic energy string", data.dynamics->states.elastic_energy_string, Quantities::energy);
    plot_combo->addData("Kinetic energy string", data.dynamics->states.kinetic_energy_string, Quantities::energy);
    plot_combo->addData("Kinetic energy arrow", data.dynamics->states.kinetic_energy_arrow, Quantities::energy);
    plot_combo->setCombination(0, 1);

    tabs->addTab(scrollArea(numbers), "Characteristics");
    tabs->addTab(plot_shapes, "Shape");
    tabs->addTab(plot_arrow, "Arrow");
    tabs->addTab(plot_stress, "Stress");
    tabs->addTab(plot_curvature, "Curvature");
    tabs->addTab(plot_energy, "Energy");
    tabs->addTab(plot_combo, "Other Plots");

    auto slider = new Slider(data.dynamics->states.time, "Time", Quantities::time);
    if(data.dynamics->arrow_departure.has_value()) {
        slider->addJumpAction("Arrow departure", data.dynamics->arrow_departure->state_idx);
    }
    slider->addJumpAction("Max. grip force (push)", std::get<1>(data.dynamics->max_forces.max_grip_force));
    slider->addJumpAction("Min. grip force (pull)", std::get<1>(data.dynamics->max_forces.min_grip_force));
    slider->addJumpAction("Max. string force", std::get<1>(data.dynamics->max_forces.max_string_force));
    for (size_t i = 0; i < data.common.layers.size(); ++i) {
        slider->addJumpAction(QString::fromStdString("Max. tension for layer: " + data.common.layers[i].name), std::get<0>(data.statics->max_stresses.max_layer_stress_tension[i]));
        slider->addJumpAction(QString::fromStdString("Max. compression for layer: " + data.common.layers[i].name), std::get<0>(data.statics->max_stresses.max_layer_stress_compression[i]));
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
