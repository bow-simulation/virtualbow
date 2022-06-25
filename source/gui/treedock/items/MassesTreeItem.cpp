#include "MassesTreeItem.hpp"
#include "gui/viewmodel/ViewModel.hpp"
#include "gui/widgets/propertytree/PropertyTreeWidget.hpp"
#include "gui/widgets/propertytree/items/DoublePropertyItem.hpp"

MassesTreeItem::MassesTreeItem(ViewModel* model)
    : TreeItem(model, "Masses", QIcon(":/icons/model-masses.svg"), TreeItemType::MASSES)
{
    arrow = new DoublePropertyItem("Arrow", Quantities::mass, DoubleRange::positive(1e-3));
    string_center = new DoublePropertyItem("String center", Quantities::mass, DoubleRange::nonNegative(1e-3));
    string_tip = new DoublePropertyItem("String tip", Quantities::mass, DoubleRange::nonNegative(1e-3));
    limb_tip = new DoublePropertyItem("Limb tip", Quantities::mass, DoubleRange::nonNegative(1e-3));

    property_tree = new PropertyTreeWidget();
    property_tree->addTopLevelItem(arrow);
    property_tree->addTopLevelItem(string_center);
    property_tree->addTopLevelItem(string_tip);
    property_tree->addTopLevelItem(limb_tip);
    property_tree->expandAll();
    setEditor(property_tree);

    updateView(nullptr);
    QObject::connect(property_tree, &QTreeWidget::itemChanged, [=]{ updateModel(this); });
    QObject::connect(model, &ViewModel::massesModified, [=](void* source){
        updateView(source);
    });
}

void MassesTreeItem::updateModel(void* source) {
    model->setMasses({
        .arrow = arrow->getValue(),
        .string_center = string_center->getValue(),
        .string_tip = string_tip->getValue(),
        .limb_tip = limb_tip->getValue()
    }, source);
}

void MassesTreeItem::updateView(void* source) {
    if(source != this) {
        QSignalBlocker blocker(property_tree);
        arrow->setValue(model->getMasses().arrow);
        string_center->setValue(model->getMasses().string_center);
        string_tip->setValue(model->getMasses().string_tip);
        limb_tip->setValue(model->getMasses().limb_tip);
    }
}

