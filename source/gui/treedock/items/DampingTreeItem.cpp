#include "DampingTreeItem.hpp"
#include "gui/viewmodel/ViewModel.hpp"
#include "gui/widgets/propertytree/PropertyTreeWidget.hpp"
#include "gui/widgets/propertytree/items/DoublePropertyItem.hpp"

DampingTreeItem::DampingTreeItem(ViewModel* model)
    : TreeItem(model, "Damping", QIcon(":/icons/model-damping.svg"), TreeItemType::DAMPING)
{
    damping_ratio_limbs = new DoublePropertyItem("Limbs", Quantities::ratio, DoubleRange::inclusive(0.0, 1.0, 1e-2));
    damping_ratio_string = new DoublePropertyItem("String", Quantities::ratio, DoubleRange::inclusive(0.0, 1.0, 1e-2));

    property_tree = new PropertyTreeWidget();
    property_tree->addTopLevelItem(damping_ratio_limbs);
    property_tree->addTopLevelItem(damping_ratio_string);
    property_tree->expandAll();
    setEditor(property_tree);

    updateView(nullptr);
    QObject::connect(property_tree, &QTreeWidget::itemChanged, [=]{ updateModel(this); });
    QObject::connect(model, &ViewModel::dampingModified, [=](void* source){
        updateView(source);
    });
}

void DampingTreeItem::updateModel(void* source) {
    model->setDamping({
        .damping_ratio_limbs = damping_ratio_limbs->getValue(),
        .damping_ratio_string = damping_ratio_string->getValue(),
    }, source);
}

void DampingTreeItem::updateView(void* source) {
    if(source != this) {
        QSignalBlocker blocker(property_tree);
        damping_ratio_limbs->setValue(model->getDamping().damping_ratio_limbs);
        damping_ratio_string->setValue(model->getDamping().damping_ratio_string);
    }
}
