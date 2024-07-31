#include "SettingsTreeItem.hpp"
#include "pre/viewmodel/ViewModel.hpp"
#include "pre/widgets/propertytree/PropertyTreeWidget.hpp"
#include "pre/widgets/propertytree/items/GroupPropertyItem.hpp"
#include "pre/widgets/propertytree/items/DoublePropertyItem.hpp"
#include "pre/widgets/propertytree/items/IntegerPropertyItem.hpp"

SettingsTreeItem::SettingsTreeItem(ViewModel* model)
    : TreeItem(model, "Settings", QIcon(":/icons/model-settings.svg"), TreeItemType::SETTINGS)
{
    auto group1 = new GroupPropertyItem("General");
    auto group2 = new GroupPropertyItem("Statics");
    auto group3 = new GroupPropertyItem("Dynamics");

    n_limb_elements = new IntegerPropertyItem("Limb elements", IntegerRange::positive(), group1);
    n_string_elements = new IntegerPropertyItem("String elements", IntegerRange::positive(), group1);
    n_draw_steps = new IntegerPropertyItem("Draw steps", IntegerRange::positive(), group2);
    arrow_clamp_force = new DoublePropertyItem("Arrow clamp force", Quantities::force, DoubleRange::unrestricted(0.1), group3);
    time_span_factor = new DoublePropertyItem("Time span factor", Quantities::ratio, DoubleRange::positive(0.01), group3);
    time_step_factor = new DoublePropertyItem("Time step factor", Quantities::ratio, DoubleRange::exclusive(0.0, 1.0, 0.01), group3);
    sampling_rate = new DoublePropertyItem("Sampling rate", Quantities::frequency, DoubleRange::positive(100.0), group3);

    property_tree = new PropertyTreeWidget();
    property_tree->addTopLevelItem(group1);
    property_tree->addTopLevelItem(group2);
    property_tree->addTopLevelItem(group3);
    property_tree->expandAll();
    setEditor(property_tree);

    updateView(nullptr);
    QObject::connect(property_tree, &QTreeWidget::itemChanged, [=]{ updateModel(this); });
    QObject::connect(model, &ViewModel::settingsModified, [=](void* source){
        updateView(source);
    });
}

void SettingsTreeItem::updateModel(void* source) {
    model->setSettings({
        .n_limb_elements = n_limb_elements->getValue(),
        .n_string_elements = n_string_elements->getValue(),
        .n_draw_steps = n_draw_steps->getValue(),
        .arrow_clamp_force = arrow_clamp_force->getValue(),
        .time_span_factor = time_span_factor->getValue(),
        .time_step_factor = time_step_factor->getValue(),
        .sampling_rate = sampling_rate->getValue()
    }, source);
}

void SettingsTreeItem::updateView(void* source) {
    if(source != this) {
        QSignalBlocker blocker(property_tree);
        n_limb_elements->setValue(model->getSettings().n_limb_elements);
        n_string_elements->setValue(model->getSettings().n_string_elements);
        n_draw_steps->setValue(model->getSettings().n_draw_steps);
        arrow_clamp_force->setValue(model->getSettings().arrow_clamp_force);
        time_span_factor->setValue(model->getSettings().time_span_factor);
        time_step_factor->setValue(model->getSettings().time_step_factor);
        sampling_rate->setValue(model->getSettings().sampling_rate);
    }
}
