#include "UnitDialog.hpp"
#include "gui/utils/UserSettings.hpp"
#include <QLabel>
#include <QComboBox>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QDialogButtonBox>

UnitEditor::UnitEditor(UnitGroup& group) {
    auto label = new QLabel(group.getName());

    auto combo = new QComboBox();
    for(int i = 0; i < group.getUnits().size(); ++i) {
        QString name = group.getUnits()[i].getName();
        // Replace empty names with "-" and add index to units list as user data.
        // Index provided by the combobox is useless because it also counts separators.
        combo->addItem(name.isEmpty() ? "-" : name, QVariant(i));
        if(i > 0 && (group.getUnits()[i].getType() != group.getUnits()[i-1].getType())) {
            combo->insertSeparator(i);    // Insert separator(s) between different types of unit (SI, US)
        }
    }
    combo->setCurrentText(group.getSelectedUnit().getName());
    combo->setFixedWidth(80);    // Magic number

    // Update unit when combobox changed
    QObject::connect(combo, QOverload<int>::of(&QComboBox::activated), this, [&, combo](int index){
        QVariant data = combo->currentData();
        const QSignalBlocker blocker(this);
        group.setSelectedIndex(data.toInt());
    });

    // Update combobox when unit changed
    QObject::connect(&group, &UnitGroup::selectionChanged, this, [&, combo](){
        QString name = group.getSelectedUnit().getName();
        if(name != combo->currentText()) {
            combo->setCurrentText(name);
        }
    });

    auto hbox = new QHBoxLayout();
    hbox->addWidget(label, 1);
    hbox->addWidget(combo, 0);
    setLayout(hbox);
}


UnitDialog::UnitDialog(QWidget* parent)
    : QDialog(parent)
{
    auto grid = new QGridLayout();
    grid->setMargin(15);

    grid->addWidget(new UnitEditor(UnitSystem::length), 0, 0);
    grid->addWidget(new UnitEditor(UnitSystem::curvature), 1, 0);
    grid->addWidget(new UnitEditor(UnitSystem::angle), 2, 0);
    grid->addWidget(new UnitEditor(UnitSystem::mass), 3, 0);
    grid->addWidget(new UnitEditor(UnitSystem::force), 4, 0);
    grid->addWidget(new UnitEditor(UnitSystem::energy), 5, 0);

    grid->addWidget(new UnitEditor(UnitSystem::time), 0, 1);
    grid->addWidget(new UnitEditor(UnitSystem::frequency), 1, 1);
    grid->addWidget(new UnitEditor(UnitSystem::elastic_modulus), 2, 1);
    grid->addWidget(new UnitEditor(UnitSystem::density), 3, 1);
    grid->addWidget(new UnitEditor(UnitSystem::linear_density), 4, 1);
    grid->addWidget(new UnitEditor(UnitSystem::linear_stiffness), 5, 1);

    grid->addWidget(new UnitEditor(UnitSystem::position), 0, 2);
    grid->addWidget(new UnitEditor(UnitSystem::velocity), 1, 2);
    grid->addWidget(new UnitEditor(UnitSystem::acceleration), 2, 2);
    grid->addWidget(new UnitEditor(UnitSystem::stress), 3, 2);
    grid->addWidget(new UnitEditor(UnitSystem::strain), 4, 2);
    grid->addWidget(new UnitEditor(UnitSystem::ratio), 5, 2);

    auto group = new QGroupBox();
    group->setLayout(grid);

    auto button_si = new QPushButton("Defaults (SI)");
    QObject::connect(button_si, &QPushButton::clicked, []{ UnitSystem::resetSI(); });

    auto button_us = new QPushButton("Defaults (US)");
    QObject::connect(button_us, &QPushButton::clicked, []{ UnitSystem::resetUS(); });

    auto buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    buttons->addButton(button_si, QDialogButtonBox::ResetRole);
    buttons->addButton(button_us, QDialogButtonBox::ResetRole);
    QObject::connect(buttons, &QDialogButtonBox::clicked, [&, buttons](QAbstractButton* button){
        UserSettings settings;
        switch(buttons->standardButton(button)) {
        case QDialogButtonBox::Ok:
            accept();
            UnitSystem::saveToSettings(settings);
            break;
        case QDialogButtonBox::Cancel:
            reject();
            UnitSystem::loadFromSettings(settings);
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
