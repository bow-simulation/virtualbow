#include "SettingsTreeItem.hpp"
#include "gui/viewmodel/DataViewModel.hpp"
#include "gui/widgets/propertytree/PropertyTreeWidget.hpp"
#include "gui/widgets/propertytree/items/GroupPropertyItem.hpp"
#include "gui/widgets/propertytree/items/DoublePropertyItem.hpp"
#include "gui/widgets/propertytree/items/IntegerPropertyItem.hpp"
#include "gui/widgets/propertytree/items/StringPropertyItem.hpp"
#include "gui/widgets/propertytree/items/ColorPropertyItem.hpp"

SettingsTreeItem::SettingsTreeItem(DataViewModel* model)
    : TreeItem("Settings", QIcon(":/icons/model-settings.svg"), TreeItemType::SETTINGS),
      model(model)
{

}

QWidget* SettingsTreeItem::createEditor() const {
    IntegerProperty limb_elements {
        .name = "Limb elements",
        .range = IntegerRange::positive(),
        .get_value = [&]() { return model->get_limb_elements(); },
        .set_value = [&](double value) { model->set_limb_elements(value); }
    };

    IntegerProperty string_elements {
        .name = "String elements",
        .range = IntegerRange::positive(),
        .get_value = [&]() { return model->get_string_elements(); },
        .set_value = [&](double value) { model->set_string_elements(value); }
    };

    IntegerProperty draw_steps {
        .name = "Draw steps",
        .range = IntegerRange::positive(),
        .get_value = [&]() { return model->get_draw_steps(); },
        .set_value = [&](double value) { model->set_draw_steps(value); }
    };

    DoubleProperty time_span_factor {
        .name = "Time span factor",
        .unit = &UnitSystem::ratio,
        .range = DoubleRange::positive(),
        .stepsize = 0.01,
        .get_value = [&]() { return model->get_time_span_factor(); },
        .set_value = [&](double value) { model->set_time_span_factor(value); }
    };

    DoubleProperty time_step_factor {
        .name = "Time step factor",
        .unit = &UnitSystem::ratio,
        .range = DoubleRange::exclusive(0.0, 1.0),
        .stepsize = 0.01,
        .get_value = [&]() { return model->get_time_step_factor(); },
        .set_value = [&](double value) { model->set_time_step_factor(value); }
    };

    DoubleProperty arrow_clamp_force {
        .name = "Arrow clamp force",
        .unit = &UnitSystem::force,
        .range = DoubleRange::nonNegative(),
        .stepsize = 0.1,
        .get_value = [&]() { return model->get_arrow_clamp_force(); },
        .set_value = [&](double value) { model->set_arrow_clamp_force(value); }
    };

    DoubleProperty sampling_rate {
        .name = "Sampling rate",
        .unit = &UnitSystem::frequency,
        .range = DoubleRange::positive(),
        .stepsize = 100.0,
        .get_value = [&]() { return model->get_sampling_rate(); },
        .set_value = [&](double value) { model->set_sampling_rate(value); }
    };

    auto group1 = new GroupPropertyItem("General");
    new IntegerPropertyItem(limb_elements, group1);
    new IntegerPropertyItem(string_elements, group1);

    auto group2 = new GroupPropertyItem("Statics");
    new IntegerPropertyItem(draw_steps, group2);

    auto group3 = new GroupPropertyItem("Dynamics");
    new DoublePropertyItem(arrow_clamp_force, group3);
    new DoublePropertyItem(time_span_factor, group3);
    new DoublePropertyItem(time_step_factor, group3);
    new DoublePropertyItem(sampling_rate, group3);

    auto tree = new PropertyTreeWidget();
    tree->addTopLevelItem(group1);
    tree->addTopLevelItem(group2);
    tree->addTopLevelItem(group3);
    tree->expandAll();

    return tree;
}
