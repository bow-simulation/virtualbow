#include "UnitDialog.hpp"
#include "Language.hpp"
#include "pre/models/units/UnitSystem.hpp"
#include "pre/utils/UserSettings.hpp"
#include <QLabel>
#include <QComboBox>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QDialogButtonBox>

UnitEditor::UnitEditor(Quantity& quantity) {
    auto label = new QLabel(quantity.getName());

    auto combo = new QComboBox();
    for(int i = 0; i < quantity.getUnits().size(); ++i) {
        QString label = quantity.getUnits()[i].getSymbol();
        QString text = quantity.getUnits()[i].getName();

        // Add unit label and tooltip text to the combobox
        // Also add index to unit list as user data (index provided by the combobox is useless because it also counts separators)
        combo->addItem(label, QVariant(i));
        combo->setItemData(combo->count() - 1, text, Qt::ToolTipRole);

        // Insert separator(s) between different types of units (SI, US)
        if(i > 0 && (quantity.getUnits()[i].getType() != quantity.getUnits()[i-1].getType())) {
            combo->insertSeparator(i);
        }
    }
    combo->setCurrentText(quantity.getUnit().getSymbol());
    combo->setFixedWidth(80);    // Magic number

    // Update unit when combobox changed
    QObject::connect(combo, QOverload<int>::of(&QComboBox::activated), this, [&, combo](int index){
        QVariant data = combo->currentData();
        const QSignalBlocker blocker(this);
        quantity.setSelectedIndex(data.toInt());
    });

    // Update combobox when unit changed
    QObject::connect(&quantity, &Quantity::unitChanged, this, [&, combo](){
        QString name = quantity.getUnit().getSymbol();
        if(name != combo->currentText()) {
            combo->setCurrentText(name);
        }
    });

    auto hbox = new QHBoxLayout();
    hbox->setContentsMargins(5, 0, 5, 0);
    hbox->addWidget(label, 1);
    hbox->addWidget(combo, 0);
    setLayout(hbox);
}


UnitDialog::UnitDialog(QWidget* parent)
    : QDialog(parent)
{
    auto grid = new QGridLayout();
    grid->setContentsMargins(15, 15, 15, 15);

    grid->addWidget(new UnitEditor(Quantities::length), 0, 0);
    grid->addWidget(new UnitEditor(Quantities::curvature), 1, 0);
    grid->addWidget(new UnitEditor(Quantities::angle), 2, 0);
    grid->addWidget(new UnitEditor(Quantities::mass), 3, 0);
    grid->addWidget(new UnitEditor(Quantities::density), 4, 0);
    grid->addWidget(new UnitEditor(Quantities::force), 5, 0);
    grid->addWidget(new UnitEditor(Quantities::stiffness), 6, 0);

    grid->addWidget(new UnitEditor(Quantities::time), 0, 1);
    grid->addWidget(new UnitEditor(Quantities::frequency), 1, 1);
    grid->addWidget(new UnitEditor(Quantities::elastic_modulus), 2, 1);
    grid->addWidget(new UnitEditor(Quantities::mass_per_force), 3, 1);
    grid->addWidget(new UnitEditor(Quantities::mass_per_energy), 4, 1);
    grid->addWidget(new UnitEditor(Quantities::linear_density), 5, 1);
    grid->addWidget(new UnitEditor(Quantities::linear_stiffness), 6, 1);

    grid->addWidget(new UnitEditor(Quantities::position), 0, 2);
    grid->addWidget(new UnitEditor(Quantities::velocity), 1, 2);
    grid->addWidget(new UnitEditor(Quantities::acceleration), 2, 2);
    grid->addWidget(new UnitEditor(Quantities::energy), 3, 2);
    grid->addWidget(new UnitEditor(Quantities::stress), 4, 2);
    grid->addWidget(new UnitEditor(Quantities::strain), 5, 2);
    grid->addWidget(new UnitEditor(Quantities::ratio), 6, 2);

    auto group = new QGroupBox();
    group->setLayout(grid);

    auto button_si = new QPushButton("Defaults (SI)");
    button_si->setToolTip(Tooltips::ResetSIDefaults);
    QObject::connect(button_si, &QPushButton::clicked, []{ Quantities::resetSI(); });

    auto button_us = new QPushButton("Defaults (US)");
    button_us->setToolTip(Tooltips::ResetUSDefaults);
    QObject::connect(button_us, &QPushButton::clicked, []{ Quantities::resetUS(); });

    auto buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    buttons->addButton(button_si, QDialogButtonBox::ResetRole);
    buttons->addButton(button_us, QDialogButtonBox::ResetRole);
    QObject::connect(buttons, &QDialogButtonBox::clicked, [&, buttons](QAbstractButton* button){
        UserSettings settings;
        switch(buttons->standardButton(button)) {
        case QDialogButtonBox::Ok:
            accept();
            Quantities::saveToSettings(settings);    // Okay: Save selection to settings
            break;
        case QDialogButtonBox::Cancel:
            reject();
            Quantities::loadFromSettings(settings);    // Cancel: Reload previous selection from settings
            break;
        default:
            // Ignore, SI and US buttons are handled elsewhere
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
