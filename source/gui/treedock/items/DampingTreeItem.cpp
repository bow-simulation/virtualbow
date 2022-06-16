#include "DampingTreeItem.hpp"
#include "gui/viewmodel/DataViewModel.hpp"
#include "gui/widgets/propertytree/PropertyTreeWidget.hpp"
#include "gui/widgets/propertytree/items/DoublePropertyItem.hpp"

DampingTreeItem::DampingTreeItem(DataViewModel* model)
    : TreeItem("Damping", QIcon(":/icons/model-damping.svg"), TreeItemType::DAMPING),
      model(model)
{
    damping_ratio_limbs = new DoublePropertyItem("Limbs", &UnitSystem::ratio, DoubleRange::inclusive(0.0, 1.0, 1e-2));
    damping_ratio_string = new DoublePropertyItem("String", &UnitSystem::ratio, DoubleRange::inclusive(0.0, 1.0, 1e-2));

    auto tree = new PropertyTreeWidget();
    tree->addTopLevelItem(damping_ratio_limbs);
    tree->addTopLevelItem(damping_ratio_string);
    tree->expandAll();
    setEditor(tree);

    updateView();
    QObject::connect(tree, &QTreeWidget::itemChanged, [&]{ updateModel(); });
}

void DampingTreeItem::updateModel() {
    model->setDamping({
        .damping_ratio_limbs = damping_ratio_limbs->getValue(),
        .damping_ratio_string = damping_ratio_string->getValue(),
    });
}

void DampingTreeItem::updateView() {
    damping_ratio_limbs->setValue(model->getDamping().damping_ratio_limbs);
    damping_ratio_string->setValue(model->getDamping().damping_ratio_string);
}
