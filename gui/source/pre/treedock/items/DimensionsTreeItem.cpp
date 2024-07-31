#include "DimensionsTreeItem.hpp"
#include "pre/viewmodel/ViewModel.hpp"
#include "pre/widgets/propertytree/PropertyTreeWidget.hpp"
#include "pre/widgets/propertytree/items/GroupPropertyItem.hpp"
#include "pre/widgets/propertytree/items/DoublePropertyItem.hpp"
#include <cmath>

DimensionsTreeItem::DimensionsTreeItem(ViewModel* model)
    : TreeItem(model, "Dimensions", QIcon(":/icons/model-dimensions.svg"), TreeItemType::DIMENSIONS)
{
    auto group1 = new GroupPropertyItem("Draw");
    auto group2 = new GroupPropertyItem("Handle");

    brace_height = new DoublePropertyItem("Brace height", Quantities::length, DoubleRange::positive(1e-3), group1);
    draw_length = new DoublePropertyItem("Draw length", Quantities::length, DoubleRange::positive(1e-3), group1);
    handle_length = new DoublePropertyItem("Length", Quantities::length, DoubleRange::positive(1e-3), group2);
    handle_setback = new DoublePropertyItem("Setback", Quantities::length, DoubleRange::unrestricted(1e-3), group2);
    handle_angle = new DoublePropertyItem("Angle", Quantities::angle, DoubleRange::unrestricted(0.25*M_PI/180), group2);

    property_tree = new PropertyTreeWidget();
    property_tree->addTopLevelItem(group1);
    property_tree->addTopLevelItem(group2);
    property_tree->expandAll();
    setEditor(property_tree);

    updateView(nullptr);
    QObject::connect(property_tree, &QTreeWidget::itemChanged, [=]{ updateModel(this); });
    QObject::connect(model, &ViewModel::dimensionsModified, [=](void* source){
        updateView(source);
    });
}

void DimensionsTreeItem::updateModel(void* source) {
    model->setDimensions({
        .brace_height = brace_height->getValue(),
        .draw_length = draw_length->getValue(),
        .handle_length = handle_length->getValue(),
        .handle_setback = handle_setback->getValue(),
        .handle_angle = handle_angle->getValue()
    }, source);
}

void DimensionsTreeItem::updateView(void* source) {
    if(source != this) {
        QSignalBlocker blocker(property_tree);
        brace_height->setValue(model->getDimensions().brace_height);
        draw_length->setValue(model->getDimensions().draw_length);
        handle_length->setValue(model->getDimensions().handle_length);
        handle_setback->setValue(model->getDimensions().handle_setback);
        handle_angle->setValue(model->getDimensions().handle_angle);
    }
}
