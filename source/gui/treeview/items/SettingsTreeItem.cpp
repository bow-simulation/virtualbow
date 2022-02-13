#include "SettingsTreeItem.hpp"
#include "gui/viewmodel/DataViewModel.hpp"
#include "gui/widgets/propertytree/PropertyTreeWidget.hpp"
#include "gui/widgets/propertytree/items/GroupPropertyItem.hpp"
#include "gui/widgets/propertytree/items/DoublePropertyItem.hpp"
#include "gui/widgets/propertytree/items/IntegerPropertyItem.hpp"

SettingsTreeItem::SettingsTreeItem(DataViewModel* model)
    : TreeItem("Settings", QIcon(":/icons/model-settings.svg"), TreeItemType::SETTINGS),
      model(model)
{
    auto group1 = new GroupPropertyItem("General");
    auto group2 = new GroupPropertyItem("Statics");
    auto group3 = new GroupPropertyItem("Dynamics");

    n_limb_elements = new IntegerPropertyItem("Limb elements", IntegerRange::positive(), group1);
    n_string_elements = new IntegerPropertyItem("String elements", IntegerRange::positive(), group1);
    n_draw_steps = new IntegerPropertyItem("Draw steps", IntegerRange::positive(), group2);
    arrow_clamp_force = new DoublePropertyItem("Arrow clamp force", &UnitSystem::force, DoubleRange::unrestricted(), 0.1, group3);
    time_span_factor = new DoublePropertyItem("Time span factor", &UnitSystem::ratio, DoubleRange::positive(), 0.01, group3);
    time_step_factor = new DoublePropertyItem("Time step factor", &UnitSystem::ratio, DoubleRange::exclusive(0.0, 1.0), 0.01, group3);
    sampling_rate = new DoublePropertyItem("Sampling rate", &UnitSystem::frequency, DoubleRange::positive(), 100.0, group3);

    auto tree = new PropertyTreeWidget();
    tree->addTopLevelItem(group1);
    tree->addTopLevelItem(group2);
    tree->addTopLevelItem(group3);
    tree->expandAll();
    setEditor(tree);

    updateView();
    QObject::connect(tree, &QTreeWidget::itemChanged, [&]{ updateModel(); });
}

void SettingsTreeItem::updateModel() {
    model->setSettings({
        .n_limb_elements = n_limb_elements->getValue(),
        .n_string_elements = n_string_elements->getValue(),
        .n_draw_steps = n_draw_steps->getValue(),
        .arrow_clamp_force = arrow_clamp_force->getValue(),
        .time_span_factor = time_span_factor->getValue(),
        .time_step_factor = time_step_factor->getValue(),
        .sampling_rate = sampling_rate->getValue()
    });
}

void SettingsTreeItem::updateView() {
    n_limb_elements->setValue(model->getSettings().n_limb_elements);
    n_string_elements->setValue(model->getSettings().n_string_elements);
    n_draw_steps->setValue(model->getSettings().n_draw_steps);
    arrow_clamp_force->setValue(model->getSettings().arrow_clamp_force);
    time_span_factor->setValue(model->getSettings().time_span_factor);
    time_step_factor->setValue(model->getSettings().time_step_factor);
    sampling_rate->setValue(model->getSettings().sampling_rate);
}
