#include "ArrowMassView.hpp"
#include "solver/BowModel.hpp"
#include "pre/widgets/DoubleSpinBox.hpp"
#include "pre/models/units/UnitSystem.hpp"
#include "pre/Language.hpp"
#include <QAbstractItemModel>
#include <QVBoxLayout>
#include <QStackedLayout>
#include <QLabel>
#include <QComboBox>
#include <cmath>

ArrowMassView::ArrowMassView(QAbstractItemModel* model, QPersistentModelIndex index) {
    auto selectionBox = new QComboBox();   
    selectionBox->addItems({"Mass", "Mass per Force", "Mass per Energy"});
    selectionBox->setToolTip(Tooltips::ArrowMassDefinition);
    selectionBox->setItemData(0, Tooltips::ArrowMassAbsolute, Qt::ToolTipRole);
    selectionBox->setItemData(1, Tooltips::ArrowMassPerForce, Qt::ToolTipRole);
    selectionBox->setItemData(2, Tooltips::ArrowMassPerEnergy, Qt::ToolTipRole);

    auto massEdit = new DoubleSpinBox(Quantities::mass, DoubleRange::positive(1e-3));
    massEdit->setToolTip(Tooltips::ArrowMassAbsolute);
    massEdit->setValue(1e-3);

    auto massPerForceEdit = new DoubleSpinBox(Quantities::mass_per_force, DoubleRange::positive(1e-6));
    massPerForceEdit->setToolTip(Tooltips::ArrowMassPerForce);
    massPerForceEdit->setValue(1e-6);

    auto massPerEnergyEdit = new DoubleSpinBox(Quantities::mass_per_energy, DoubleRange::positive(1e-4));
    massPerEnergyEdit->setToolTip(Tooltips::ArrowMassPerEnergy);
    massPerEnergyEdit->setValue(1e-4);

    auto stack = new QStackedLayout();
    stack->addWidget(massEdit);
    stack->addWidget(massPerForceEdit);
    stack->addWidget(massPerEnergyEdit);

    auto vbox = new QVBoxLayout();
    vbox->setContentsMargins(0, 0, 0, 0);
    vbox->addWidget(selectionBox);
    vbox->addLayout(stack);
    setLayout(vbox);

    // Update view according to the data in the model
    auto updateView = [=]() {
        // Set selection according to the type of mass definition
        ArrowMass mass = model->data(index, Qt::DisplayRole).value<ArrowMass>();
        selectionBox->setCurrentIndex(mass.index());

        // Assign value to the correct editor according to the type of mass definition
        if(auto value = std::get_if<Mass>(&mass)) {
            massEdit->setValue(value->value);
        }
        else if(auto value = std::get_if<MassPerForce>(&mass)) {
            massPerForceEdit->setValue(value->value);
        }
        else if(auto value = std::get_if<MassPerEnergy>(&mass)) {
            massPerEnergyEdit->setValue(value->value);
        }
        else {
            throw std::invalid_argument("Unknown variant");
        }
    };

    // Update model according to data in the view
    auto updateModel = [=]() {
        switch(selectionBox->currentIndex()) {
        case 0:
            model->setData(index, QVariant::fromValue<ArrowMass>(Mass{ .value = massEdit->value() }));
            break;

        case 1:
            model->setData(index, QVariant::fromValue<ArrowMass>(MassPerForce{ .value = massPerForceEdit->value() }));
            break;

        case 2:
            model->setData(index, QVariant::fromValue<ArrowMass>(MassPerEnergy{ .value = massPerEnergyEdit->value() }));
            break;

        default:
            throw std::invalid_argument("Unknown variant");
        }
    };

    // Activate the correct editor according to the selected mass definition
    QObject::connect(selectionBox, &QComboBox::currentIndexChanged, stack, &QStackedLayout::setCurrentIndex);

    // Initialize view from model once
    updateView();

    // Keep model up to date on changes
    QObject::connect(selectionBox, &QComboBox::currentIndexChanged, this, updateModel);
    QObject::connect(massEdit, &DoubleSpinBox::contentModified, this, updateModel);
    QObject::connect(massPerForceEdit, &DoubleSpinBox::contentModified, this, updateModel);
    QObject::connect(massPerEnergyEdit, &DoubleSpinBox::contentModified, this, updateModel);
}
