#include "StringTreeItem.hpp"
#include "gui/viewmodel/ViewModel.hpp"
#include "gui/widgets/propertytree/PropertyTreeWidget.hpp"
#include "gui/widgets/propertytree/items/DoublePropertyItem.hpp"
#include "gui/widgets/propertytree/items/IntegerPropertyItem.hpp"

StringTreeItem::StringTreeItem(ViewModel* model)
    : TreeItem(model, "String", QIcon(":/icons/model-string.svg"), TreeItemType::STRING)
{
    strand_stiffness = new DoublePropertyItem("Strand stiffness", Quantities::linear_stiffness, DoubleRange::positive(1.0));
    strand_density = new DoublePropertyItem("Strand density", Quantities::linear_density, DoubleRange::positive(1e-4));
    n_strands = new IntegerPropertyItem("Strands", IntegerRange::positive());

    property_tree = new PropertyTreeWidget();
    property_tree->addTopLevelItem(strand_stiffness);
    property_tree->addTopLevelItem(strand_density);
    property_tree->addTopLevelItem(n_strands);
    property_tree->expandAll();
    setEditor(property_tree);

    updateView(nullptr);
    QObject::connect(property_tree, &QTreeWidget::itemChanged, [=]{ updateModel(this); });
    QObject::connect(model, &ViewModel::stringModified, [=](void* source){
        updateView(source);
    });
}

void StringTreeItem::updateModel(void* source) {
    model->setString({
        .strand_stiffness = strand_stiffness->getValue(),
        .strand_density = strand_density->getValue(),
        .n_strands = n_strands->getValue()
    }, source);
}

void StringTreeItem::updateView(void* source) {
    if(source != this) {
        QSignalBlocker blocker(property_tree);
        strand_stiffness->setValue(model->getString().strand_stiffness);
        strand_density->setValue(model->getString().strand_density);
        n_strands->setValue(model->getString().n_strands);
    }
}
