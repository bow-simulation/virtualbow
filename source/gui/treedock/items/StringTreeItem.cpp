#include "StringTreeItem.hpp"
#include "gui/viewmodel/DataViewModel.hpp"
#include "gui/widgets/propertytree/PropertyTreeWidget.hpp"
#include "gui/widgets/propertytree/items/DoublePropertyItem.hpp"
#include "gui/widgets/propertytree/items/IntegerPropertyItem.hpp"

StringTreeItem::StringTreeItem(DataViewModel* model)
    : TreeItem("String", QIcon(":/icons/model-string.svg"), TreeItemType::STRING),
      model(model)
{
    strand_stiffness = new DoublePropertyItem("Strand stiffness", &UnitSystem::linear_stiffness, DoubleRange::positive(1.0));
    strand_density = new DoublePropertyItem("Strand density", &UnitSystem::linear_density, DoubleRange::positive(1e-4));
    n_strands = new IntegerPropertyItem("Strands", IntegerRange::positive());

    auto tree = new PropertyTreeWidget();
    tree->addTopLevelItem(strand_stiffness);
    tree->addTopLevelItem(strand_density);
    tree->addTopLevelItem(n_strands);
    tree->expandAll();
    setEditor(tree);

    updateView();
    QObject::connect(tree, &QTreeWidget::itemChanged, [&]{ updateModel(); });
}

void StringTreeItem::updateModel() {
    model->setString({
        .strand_stiffness = strand_stiffness->getValue(),
        .strand_density = strand_density->getValue(),
        .n_strands = n_strands->getValue()
    });
}

void StringTreeItem::updateView() {
    strand_stiffness->setValue(model->getString().strand_stiffness);
    strand_density->setValue(model->getString().strand_density);
    n_strands->setValue(model->getString().n_strands);
}
