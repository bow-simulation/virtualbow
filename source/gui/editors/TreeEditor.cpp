#include "TreeEditor.hpp"
#include "TreeItem.hpp"
#include "gui/dialogs/CommentDialog.hpp"
#include "gui/dialogs/ProfileDialog.hpp"
#include "gui/dialogs/WidthDialog.hpp"
#include "gui/dialogs/LayerDialog.hpp"
#include "gui/dialogs/SettingsDialog.hpp"
#include "gui/dialogs/StringDialog.hpp"
#include "gui/dialogs/MassesDialog.hpp"
#include "gui/dialogs/DampingDialog.hpp"
#include "gui/dialogs/DimensionsDialog.hpp"

TreeEditor::TreeEditor()
{
    new TreeItem<CommentDialog, std::string>(this, data.meta.comments, "Comments", QIcon(":/icons/comments"));
    new TreeItem<SettingsDialog, Settings>(this, data.settings, "Settings", QIcon(":/icons/settings"));
    new TreeItem<ProfileDialog, Series>(this, data.profile, "Profile", QIcon(":/icons/profile"));
    new TreeItem<WidthDialog, Series>(this, data.width, "Width", QIcon(":/icons/width"));
    new TreeItem<LayerDialog, Layers>(this, data.layers, "Layers", QIcon(":/icons/layers"));
    new TreeItem<StringDialog, String>(this, data.string, "String", QIcon(":/icons/string"));
    new TreeItem<MassesDialog, Masses>(this, data.masses, "Masses", QIcon(":/icons/masses"));
    new TreeItem<DampingDialog, Damping>(this, data.damping, "Damping", QIcon(":/icons/damping"));
    new TreeItem<DimensionsDialog, Dimensions>(this, data.dimensions, "Dimensions", QIcon(":/icons/dimensions"));

    QObject::connect(this, &QTreeWidget::itemActivated, [](QTreeWidgetItem* item, int column)
    {
        auto action = dynamic_cast<Action*>(item);
        if(action != nullptr)
            action->performAction();
    });

    this->setHeaderLabel("Model");
    this->expandAll();
    this->setItemsExpandable(false);    // Todo: Why is the expansion symbol still visible? (on KDE Desktop at least)
}

const InputData& TreeEditor::getData() const
{
    return data;
}

void TreeEditor::setData(const InputData& data)
{
    this->data = data;
}
