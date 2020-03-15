#include "CharacteristicNumbers.hpp"

NumberGrid::NumberGrid()
    : columnLayout(new QHBoxLayout()),
    currentColumn(nullptr),
    currentGrid(nullptr)
{
    columnLayout->setSpacing(20);
    columnLayout->setMargin(20);
    columnLayout->addStretch();
    this->setLayout(columnLayout);
}

void NumberGrid::addColumn()
{
    // Create new column
    currentColumn = new QVBoxLayout();
    currentColumn->addStretch();

    // Forget old grid
    currentGrid = nullptr;

    // Insert before horizontal stretch
    int i = columnLayout->count() - 1;
    columnLayout->insertLayout(i, currentColumn);
}

void NumberGrid::addGroup(const QString& text)
{
    if (currentColumn == nullptr)
        addColumn();

    currentGrid = new QGridLayout();
    // ...

    auto group = new QGroupBox(text);
    group->setLayout(currentGrid);

    // Insert before vertical stretch
    int i = currentColumn->count() - 1;
    currentColumn->insertWidget(i, group);
}

void NumberGrid::addValue(const QString& text, double value)
{
    if (currentColumn == nullptr)
        addColumn();

    if (currentGrid == nullptr)
        addGroup("Default");

    auto label = new QLabel(text);
    auto edit = new QLineEdit(QLocale().toString(value));
    edit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    edit->setReadOnly(true);

    int i = currentGrid->rowCount();
    currentGrid->addWidget(label, i, 0, Qt::AlignRight);
    currentGrid->addWidget(edit, i, 1);
}

auto absMaxOrder = [](double a, double b) {
    return std::abs(a) < std::abs(b);
};

double maxAbsForce(const std::vector<double>& vec)
{
    return *std::max_element(vec.begin(), vec.end(), absMaxOrder);
};

double maxAbsStress(const LayerProperties& layer, const BowStates& states)
{
    double sigma_max = 0.0;
    for (int i = 0; i < states.time.size(); ++i)
    {
        VectorXd sigma_back = layer.He_back * states.epsilon[i] + layer.Hk_back * states.kappa[i];
        VectorXd sigma_belly = layer.He_belly * states.epsilon[i] + layer.Hk_belly * states.kappa[i];
        
        sigma_max = std::max({
            sigma_max,
            sigma_back.minCoeff(),
            sigma_back.maxCoeff(),
            sigma_belly.minCoeff(),
            sigma_belly.maxCoeff()
        }, absMaxOrder);
    }

    return sigma_max;
};

StaticNumbers::StaticNumbers(const InputData& input, const LimbProperties& limb, const StaticData& statics)
{
    addColumn();
    addGroup("Performance");
    addValue("Final draw force [N]", statics.final_draw_force);
    addValue("Drawing work [J]", statics.drawing_work);
    addValue("Storage ratio", statics.storage_ratio);

    addGroup("Properties");
    addValue("Limb mass [kg]", statics.limb_mass);
    addValue("String mass [kg]", statics.string_mass);
    addValue("String length [m]", statics.string_length);

    addColumn();
    addGroup("Maximum absolute stresses");
    for(int i = 0; i < limb.layers.size(); ++i) {
        addValue(QString::fromStdString(input.layers[i].name + " [Pa]"), maxAbsStress(limb.layers[i], statics.states));
    }

    addGroup("Maximum absolute forces");
    addValue("String force (total) [N]", maxAbsForce(statics.states.string_force));
    addValue("String force (strand) [N]", maxAbsForce(statics.states.strand_force));
    addValue("Grip force [N]", maxAbsForce(statics.states.grip_force));
}

DynamicNumbers::DynamicNumbers(const InputData& input, const LimbProperties& limb, const DynamicData& dynamics)
{
    addColumn();
    addGroup("Performance");
    addValue("Final arrow velocity [m/s]", dynamics.final_arrow_velocity);
    addValue("Final arrow energy [J]", dynamics.final_arrow_energy);
    addValue("Efficiency", dynamics.efficiency);

    addColumn();
    addGroup("Maximum absolute stresses");
    for (int i = 0; i < limb.layers.size(); ++i) {
        addValue(QString::fromStdString(input.layers[i].name + " [Pa]"), maxAbsStress(limb.layers[i], dynamics.states));
    }

    addGroup("Maximum absolute forces");
    addValue("String force (total) [N]", maxAbsForce(dynamics.states.string_force));
    addValue("String force (strand) [N]", maxAbsForce(dynamics.states.strand_force));
    addValue("Grip force [N]", maxAbsForce(dynamics.states.grip_force));
}