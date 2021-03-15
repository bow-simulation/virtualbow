#include "UnitsDialog.hpp"

UnitEditor::UnitEditor(const QString& text, const QList<Unit>& units)
    : units(units),
      combo(new QComboBox())
{
    auto label = new QLabel(text);
    label->setAlignment(Qt::AlignRight);

    for(int i = 0; i < units.size(); ++i) {
        // Add index in units list as user data. Index provided by the combobox index is useless because it also counts separators.
        combo->addItem(units[i].getName(), QVariant(i));
        if(i > 0 && (units[i].getType() != units[i-1].getType())) {
            // Insert separator(s) between different types of unit (SI, US)
            combo->insertSeparator(i);
        }
    }
    combo->setFixedWidth(80);    // Magic number

    auto hbox = new QHBoxLayout();
    hbox->addWidget(label, 1);
    hbox->addWidget(combo, 0);
    setLayout(hbox);
}

Unit UnitEditor::getUnit() const {
    QVariant data = combo->currentData();
    return units[data.toInt()];
}

void UnitEditor::setUnit(const Unit& unit) {
    combo->setCurrentText(unit.getName());
}

UnitsDialog::UnitsDialog(QWidget* parent)
    : QDialog(parent)
{
    auto grid = new QGridLayout();
    grid->setHorizontalSpacing(20);
    grid->setMargin(20);

    grid->addWidget(edit_length, 0, 0);
    grid->addWidget(edit_angle, 1, 0);
    grid->addWidget(edit_mass, 2, 0);
    grid->addWidget(edit_force, 3, 0);
    grid->addWidget(edit_time, 4, 0);
    grid->addWidget(edit_energy, 5, 0);

    grid->addWidget(edit_position, 0, 1);
    grid->addWidget(edit_velocity, 1, 1);
    grid->addWidget(edit_acceleration, 2, 1);
    grid->addWidget(edit_elastic_modulus, 3, 1);
    grid->addWidget(edit_density, 4, 1);
    grid->addWidget(edit_linear_stiffness, 5, 1);

    grid->addWidget(edit_linear_density, 0, 2);
    grid->addWidget(edit_strain, 1, 2);
    grid->addWidget(edit_curvature, 2, 2);
    grid->addWidget(edit_stress, 3, 2);
    grid->addWidget(edit_damping_ratio, 4, 2);
    grid->addWidget(edit_relative_position, 5, 2);

    auto group = new QGroupBox();
    group->setLayout(grid);

    auto buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Reset);
    QObject::connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    QObject::connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    QObject::connect(buttons, &QDialogButtonBox::clicked, [&, buttons](QAbstractButton* button){
        switch(buttons->standardButton(button)) {
        case QDialogButtonBox::Ok:
            accept();
            break;
        case QDialogButtonBox::Cancel:
            reject();
            break;
        case QDialogButtonBox::Reset:
            setUnits(UnitSystem());
            break;
        }
    });

    auto vbox = new QVBoxLayout();
    vbox->setSizeConstraint(QLayout::SetFixedSize);
    vbox->addWidget(group, 1);
    vbox->addWidget(buttons, 0);

    setWindowTitle("Units");
    setLayout(vbox);
}

UnitSystem UnitsDialog::getUnits() const {
    return {
        .length = edit_length->getUnit(),
        .angle = edit_angle->getUnit(),
        .mass = edit_mass->getUnit(),
        .force = edit_force->getUnit(),
        .time = edit_time->getUnit(),
        .energy = edit_energy->getUnit(),
        .position = edit_position->getUnit(),
        .velocity = edit_velocity->getUnit(),
        .acceleration = edit_acceleration->getUnit(),
        .elastic_modulus = edit_elastic_modulus->getUnit(),
        .density = edit_density->getUnit(),
        .linear_stiffness = edit_linear_stiffness->getUnit(),
        .linear_density = edit_linear_density->getUnit(),
        .strain = edit_strain->getUnit(),
        .curvature = edit_curvature->getUnit(),
        .stress = edit_stress->getUnit(),
        .damping_ratio = edit_damping_ratio->getUnit(),
        .relative_position = edit_relative_position->getUnit()
    };
}

void UnitsDialog::setUnits(const UnitSystem& units) {
    edit_length->setUnit(units.length);
    edit_angle->setUnit(units.angle);
    edit_mass->setUnit(units.mass);
    edit_force->setUnit(units.force);
    edit_time->setUnit(units.time);
    edit_energy->setUnit(units.energy);
    edit_position->setUnit(units.position);
    edit_velocity->setUnit(units.velocity);
    edit_acceleration->setUnit(units.acceleration);
    edit_elastic_modulus->setUnit(units.elastic_modulus);
    edit_density->setUnit(units.density);
    edit_linear_stiffness->setUnit(units.linear_stiffness);
    edit_linear_density->setUnit(units.linear_density);
    edit_strain->setUnit(units.strain);
    edit_curvature->setUnit(units.curvature);
    edit_stress->setUnit(units.stress);
    edit_damping_ratio->setUnit(units.damping_ratio);
    edit_relative_position->setUnit(units.length);
}
