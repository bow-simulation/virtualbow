#include "MassesTreeItem.hpp"
#include "gui/viewmodel/DataViewModel.hpp"
#include "gui/widgets/propertytree/PropertyTreeWidget.hpp"
#include "gui/widgets/propertytree/items/DoublePropertyItem.hpp"

MassesTreeItem::MassesTreeItem(DataViewModel* model)
    : TreeItem("Masses", QIcon(":/icons/model-masses.svg"), TreeItemType::MASSES),
      model(model)
{
    arrow = new DoublePropertyItem("Arrow", &UnitSystem::mass, DoubleRange::positive(), 0.001);
    string_center = new DoublePropertyItem("String center", &UnitSystem::mass, DoubleRange::nonNegative(), 0.001);
    string_tip = new DoublePropertyItem("String tip", &UnitSystem::mass, DoubleRange::nonNegative(), 0.001);
    limb_tip = new DoublePropertyItem("Limb tip", &UnitSystem::mass, DoubleRange::nonNegative(), 0.001);

    auto tree = new PropertyTreeWidget();
    tree->addTopLevelItem(arrow);
    tree->addTopLevelItem(string_center);
    tree->addTopLevelItem(string_tip);
    tree->addTopLevelItem(limb_tip);
    tree->expandAll();
    setEditor(tree);

    updateView();
    QObject::connect(tree, &QTreeWidget::itemChanged, [&]{ updateModel(); });
}

void MassesTreeItem::updateModel() {
    model->setMasses({
        .arrow = arrow->getValue(),
        .string_center = string_center->getValue(),
        .string_tip = string_tip->getValue(),
        .limb_tip = limb_tip->getValue()
    });
}

void MassesTreeItem::updateView() {
    arrow->setValue(model->getMasses().arrow);
    string_center->setValue(model->getMasses().string_center);
    string_tip->setValue(model->getMasses().string_tip);
    limb_tip->setValue(model->getMasses().limb_tip);
}

