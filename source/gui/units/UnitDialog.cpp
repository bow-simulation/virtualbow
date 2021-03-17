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
            // Insert separator(s) between different types of unit (SI, US)
            combo->insertSeparator(i);
        }
    }
    combo->setCurrentText(group.getSelectedUnit().getName());
    combo->setFixedWidth(80);    // Magic number

    QObject::connect(combo, QOverload<int>::of(&QComboBox::activated), this, [&, combo](int index){
        QVariant data = combo->currentData();
        group.setSelectedIndex(data.toInt());
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
    grid->addWidget(new UnitEditor(units.angle), 1, 0);
    grid->addWidget(new UnitEditor(units.mass), 2, 0);
    grid->addWidget(new UnitEditor(units.force), 3, 0);
    grid->addWidget(new UnitEditor(units.time), 4, 0);
    grid->addWidget(new UnitEditor(units.energy), 5, 0);

    grid->addWidget(new UnitEditor(units.position), 0, 1);
    grid->addWidget(new UnitEditor(units.velocity), 1, 1);
    grid->addWidget(new UnitEditor(units.acceleration), 2, 1);
    grid->addWidget(new UnitEditor(units.elastic_modulus), 3, 1);
    grid->addWidget(new UnitEditor(units.density), 4, 1);
    grid->addWidget(new UnitEditor(units.linear_stiffness), 5, 1);

    grid->addWidget(new UnitEditor(units.linear_density), 0, 2);
    grid->addWidget(new UnitEditor(units.strain), 1, 2);
    grid->addWidget(new UnitEditor(units.curvature), 2, 2);
    grid->addWidget(new UnitEditor(units.stress), 3, 2);
    grid->addWidget(new UnitEditor(units.ratio), 4, 2);
    grid->addWidget(new UnitEditor(units.frequency), 5, 2);

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
            qInfo() << "TODO: Reset units";
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
