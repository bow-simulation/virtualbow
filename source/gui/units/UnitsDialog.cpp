#include "UnitsDialog.hpp"

UnitEditor::UnitEditor(const QString& text, Unit& binding, const QList<Unit>& units)
    : binding(binding),
      units(units)
{
    auto label = new QLabel(text);
    label->setAlignment(Qt::AlignRight);

    auto combo = new QComboBox();
    for(int i = 0; i < units.size(); ++i) {
        combo->addItem(units[i].getName());
        // Insert separator(s) between different types of unit (SI, US)
        if(i > 0 && (units[i].getType() != units[i-1].getType())) {
            combo->insertSeparator(i);
        }
    }
    combo->setCurrentText(binding.getName());
    combo->setFixedWidth(80);    // Magic number

    // TODO: Use QComboBox::textActivated in Qt >= 5.14
    QObject::connect(combo, QOverload<const QString&>::of(&QComboBox::activated), this, [&](const QString& text) {
        // Search selected text in unit list. Using the index would include the separator and not map with the units list.
        for(auto& unit: this->units) {
            if(unit.getName() == text) {
                this->binding = unit;    // Assign selected unit to the unit referenced by this widget
            }
        }
    });

    auto hbox = new QHBoxLayout();
    hbox->addWidget(label, 1);
    hbox->addWidget(combo, 0);
    this->setLayout(hbox);
}

UnitsDialog::UnitsDialog(QWidget* parent, const UnitSystem& defaults)
    : QDialog(parent),
      units(defaults)
{
    auto grid = new QGridLayout();
    grid->setHorizontalSpacing(20);
    grid->setMargin(20);

    grid->addWidget(new UnitEditor("Length", units.length, UnitOptions::LENGTH), 0, 0);
    grid->addWidget(new UnitEditor("Angle", units.angle, UnitOptions::ANGLE), 1, 0);
    grid->addWidget(new UnitEditor("Mass", units.mass, UnitOptions::MASS), 2, 0);
    grid->addWidget(new UnitEditor("Force", units.force, UnitOptions::FORCE), 3, 0);
    grid->addWidget(new UnitEditor("Time", units.time, UnitOptions::TIME), 4, 0);
    grid->addWidget(new UnitEditor("Energy", units.energy, UnitOptions::ENERGY), 5, 0);

    grid->addWidget(new UnitEditor("Position", units.position, UnitOptions::POSITION), 0, 1);
    grid->addWidget(new UnitEditor("Velocity", units.velocity, UnitOptions::VELOCITY), 1, 1);
    grid->addWidget(new UnitEditor("Acceleration", units.acceleration, UnitOptions::ACCELERATION), 2, 1);
    grid->addWidget(new UnitEditor("Elastic modulus", units.elastic_modulus, UnitOptions::ELASTIC_MODULUS), 3, 1);
    grid->addWidget(new UnitEditor("Density", units.density, UnitOptions::DENSITY), 4, 1);
    grid->addWidget(new UnitEditor("Linear stiffness", units.linear_stiffness, UnitOptions::LINEAR_STIFFNESS), 5, 1);

    grid->addWidget(new UnitEditor("Linear density", units.linear_density, UnitOptions::LINEAR_DENSITY), 0, 2);
    grid->addWidget(new UnitEditor("Strain", units.strain, UnitOptions::STRAIN), 1, 2);
    grid->addWidget(new UnitEditor("Curvature", units.curvature, UnitOptions::CURVATURE), 2, 2);
    grid->addWidget(new UnitEditor("Stress", units.stress, UnitOptions::STRESS), 3, 2);
    grid->addWidget(new UnitEditor("Damping ratio", units.damping_ratio, UnitOptions::DAMPING_RATIO), 4, 2);
    grid->addWidget(new UnitEditor("Relative position", units.relative_position, UnitOptions::RELATIVE_POSITION), 5, 2);

    auto group = new QGroupBox();
    group->setLayout(grid);

    auto buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Reset);
    QObject::connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    QObject::connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    auto vbox = new QVBoxLayout();
    vbox->setSizeConstraint(QLayout::SetFixedSize);
    vbox->addWidget(group, 1);
    vbox->addWidget(buttons, 0);

    this->setWindowTitle("Units");
    this->setLayout(vbox);
}

const UnitSystem& UnitsDialog::getUnits() const {
    return units;
}
