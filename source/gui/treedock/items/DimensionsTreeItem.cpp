#include "DimensionsTreeItem.hpp"
#include "gui/viewmodel/DataViewModel.hpp"
#include "gui/widgets/propertytree/PropertyTreeWidget.hpp"
#include "gui/widgets/propertytree/items/GroupPropertyItem.hpp"
#include "gui/widgets/propertytree/items/DoublePropertyItem.hpp"

DimensionsTreeItem::DimensionsTreeItem(DataViewModel* model)
    : TreeItem("Dimensions", QIcon(":/icons/model-dimensions.svg"), TreeItemType::DIMENSIONS),
      model(model)
{
    auto group1 = new GroupPropertyItem("Draw");
    auto group2 = new GroupPropertyItem("Handle");

    brace_height = new DoublePropertyItem("Brace height", &UnitSystem::length, DoubleRange::positive(1e-3), group1);
    draw_length = new DoublePropertyItem("Draw length", &UnitSystem::length, DoubleRange::positive(1e-3), group1);
    handle_length = new DoublePropertyItem("Length", &UnitSystem::length, DoubleRange::positive(1e-3), group2);
    handle_setback = new DoublePropertyItem("Setback", &UnitSystem::length, DoubleRange::unrestricted(1e-3), group2);
    handle_angle = new DoublePropertyItem("Angle", &UnitSystem::angle, DoubleRange::unrestricted(1e-3), group2);

    auto tree = new PropertyTreeWidget();
    tree->addTopLevelItem(group1);
    tree->addTopLevelItem(group2);
    tree->expandAll();
    setEditor(tree);

    updateView();
    QObject::connect(tree, &QTreeWidget::itemChanged, [&]{ updateModel(); });
}

void DimensionsTreeItem::updateModel() {
    model->setDimensions({
        .brace_height = brace_height->getValue(),
        .draw_length = draw_length->getValue(),
        .handle_length = handle_length->getValue(),
        .handle_setback = handle_setback->getValue(),
        .handle_angle = handle_angle->getValue()
    });
}

void DimensionsTreeItem::updateView() {
    brace_height->setValue(model->getDimensions().brace_height);
    draw_length->setValue(model->getDimensions().draw_length);
    handle_length->setValue(model->getDimensions().handle_length);
    handle_setback->setValue(model->getDimensions().handle_setback);
    handle_angle->setValue(model->getDimensions().handle_angle);
}
