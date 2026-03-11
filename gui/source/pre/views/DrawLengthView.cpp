#include "DrawLengthView.hpp"
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

DrawLengthView::DrawLengthView(QAbstractItemModel* model, QPersistentModelIndex index) {
    auto selectionBox = new QComboBox();
    selectionBox->addItems({"Standard", "AMO"});
    selectionBox->setToolTip(Tooltips::DrawLengthDefinition);
    selectionBox->setItemData(0, Tooltips::DrawLengthStandard, Qt::ToolTipRole);
    selectionBox->setItemData(1, Tooltips::DrawLengthAMO, Qt::ToolTipRole);

    auto lengthEdit = new DoubleSpinBox(Quantities::length, DoubleRange::nonNegative(1e-3));
    lengthEdit->setValue(1e-3);

    auto vbox = new QVBoxLayout();
    vbox->setContentsMargins(0, 0, 0, 0);
    vbox->addWidget(selectionBox);
    vbox->addWidget(lengthEdit);
    setLayout(vbox);

    // Sets the tooltip of the editor according to the selection
    auto updateTooltip = [=]() {
        switch(selectionBox->currentIndex()) {
        case 0:
            lengthEdit->setToolTip(Tooltips::DrawLengthStandard);
            break;

        case 1:
            lengthEdit->setToolTip(Tooltips::DrawLengthAMO);
            break;

        default:
            throw std::invalid_argument("Unknown variant");
        }
    };

    // Update view according to the data in the model
    auto updateView = [=]() {
        // Set selection according to the type of length definition
        DrawLength length = model->data(index, Qt::DisplayRole).value<DrawLength>();
        selectionBox->setCurrentIndex(length.index());

        // Assign value to the correct editor according to the type of length definition
        if(auto value = std::get_if<StandardDrawLength>(&length)) {
            lengthEdit->setValue(value->value);
        }
        else if(auto value = std::get_if<AMODrawLength>(&length)) {
            lengthEdit->setValue(value->value);
        }
         else {
            throw std::invalid_argument("Unknown variant");
        }

        updateTooltip();
    };

    // Update model according to data in the view
    auto updateModel = [=]() {
        switch(selectionBox->currentIndex()) {
        case 0:
            model->setData(index, QVariant::fromValue<DrawLength>(StandardDrawLength{ .value = lengthEdit->value() }));
            break;

        case 1:
            model->setData(index, QVariant::fromValue<DrawLength>(AMODrawLength{ .value = lengthEdit->value() }));
            break;

        default:
            throw std::invalid_argument("Unknown variant");
        }
    };

    // Show the correct tooltip according to the selected definition
    QObject::connect(selectionBox, &QComboBox::currentIndexChanged, this, updateTooltip);

    // Initialize view from model once
    updateView();

    // Keep model up to date on changes
    QObject::connect(selectionBox, &QComboBox::currentIndexChanged, this, updateModel);
    QObject::connect(lengthEdit, &DoubleSpinBox::contentModified, this, updateModel);
}
