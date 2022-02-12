#include "SettingsTreeItem.hpp"
#include "gui/viewmodel/DataViewModel.hpp"
#include "gui/widgets/propertytree/PropertyTreeWidget.hpp"
#include "gui/widgets/propertytree/items/GroupTreeItem.hpp"
#include "gui/widgets/propertytree/items/DoubleTreeItem.hpp"
#include "gui/widgets/propertytree/items/IntegerTreeItem.hpp"
#include "gui/widgets/propertytree/items/StringTreeItem.hpp"
#include "gui/widgets/propertytree/items/ColorTreeItem.hpp"

SettingsTreeItem::SettingsTreeItem(DataViewModel* model)
    : TreeItem("Settings", QIcon(":/icons/model-settings.svg"), TreeItemType::SETTINGS),
      model(model)
{

}

QWidget* SettingsTreeItem::createEditor() const {
    auto item_group1 = new GroupTreeItem("General");
    new IntegerTreeItem2("Limb elements", item_group1);
    new IntegerTreeItem2("String elements", item_group1);

    auto item_group2 = new GroupTreeItem("Statics");
    new IntegerTreeItem2("Draw steps", item_group2);

    auto item_group3 = new GroupTreeItem("Dynamics");
    new DoubleTreeItem("Arrow clamp force", item_group3);
    new DoubleTreeItem("Time span factor", item_group3);
    new DoubleTreeItem("Time step factor", item_group3);
    new DoubleTreeItem("Sampling rate", item_group3);

    auto tree = new PropertyTreeWidget();
    tree->addTopLevelItem(item_group1);
    tree->addTopLevelItem(item_group2);
    tree->addTopLevelItem(item_group3);
    tree->expandAll();

    return tree;
}
