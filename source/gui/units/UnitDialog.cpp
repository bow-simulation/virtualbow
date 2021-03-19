#include "UnitDialog.hpp"

UnitEditor::UnitEditor(UnitGroup& group) {
    auto label = new QLabel(group.getName());
    label->setAlignment(Qt::AlignRight);

    QComboBox* combo = new QComboBox();
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
        this->blockSignals(true);
        group.setSelectedIndex(data.toInt());
        this->blockSignals(false);
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


UnitDialog::UnitDialog(QWidget* parent, UnitSystem& units)
    : QDialog(parent)
{
    auto grid = new QGridLayout();
    grid->setHorizontalSpacing(20);
    grid->setMargin(20);

    grid->addWidget(new UnitEditor(units.length), 0, 0);
    grid->addWidget(new UnitEditor(units.curvature), 1, 0);
    grid->addWidget(new UnitEditor(units.angle), 2, 0);
    grid->addWidget(new UnitEditor(units.mass), 3, 0);
    grid->addWidget(new UnitEditor(units.force), 4, 0);
    grid->addWidget(new UnitEditor(units.energy), 5, 0);

    grid->addWidget(new UnitEditor(units.time), 0, 1);
    grid->addWidget(new UnitEditor(units.frequency), 1, 1);
    grid->addWidget(new UnitEditor(units.elastic_modulus), 2, 1);
    grid->addWidget(new UnitEditor(units.density), 3, 1);
    grid->addWidget(new UnitEditor(units.linear_density), 4, 1);
    grid->addWidget(new UnitEditor(units.linear_stiffness), 5, 1);

    grid->addWidget(new UnitEditor(units.position), 0, 2);
    grid->addWidget(new UnitEditor(units.velocity), 1, 2);
    grid->addWidget(new UnitEditor(units.acceleration), 2, 2);
    grid->addWidget(new UnitEditor(units.stress), 3, 2);
    grid->addWidget(new UnitEditor(units.strain), 4, 2);
    grid->addWidget(new UnitEditor(units.ratio), 5, 2);

    auto group = new QGroupBox();
    group->setLayout(grid);

    auto buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Reset);
    QObject::connect(buttons, &QDialogButtonBox::clicked, [&, buttons](QAbstractButton* button){
        switch(buttons->standardButton(button)) {
        case QDialogButtonBox::Ok:
            accept();
            units.saveToSettings();
            break;
        case QDialogButtonBox::Cancel:
            reject();
            units.loadFromSettings();
            break;
        case QDialogButtonBox::Reset:
            units.resetDefaults();
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
