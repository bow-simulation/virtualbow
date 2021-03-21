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

TreeEditor::TreeEditor(const UnitSystem& units)
{
    new TreeItem<CommentDialog, std::string>(this, units, data.comment, "Comments", QIcon(":/icons/model-comments.svg"));
    new TreeItem<SettingsDialog, Settings>(this, units, data.settings, "Settings", QIcon(":/icons/model-settings.svg"));
    new TreeItem<DimensionsDialog, Dimensions>(this, units, data.dimensions, "Dimensions", QIcon(":/icons/model-dimensions.svg"));
    new TreeItem<ProfileDialog, MatrixXd>(this, units, data.profile, "Profile", QIcon(":/icons/model-profile.svg"));
    new TreeItem<WidthDialog, MatrixXd>(this, units, data.width, "Width", QIcon(":/icons/model-width.svg"));
    new TreeItem<LayerDialog, std::vector<Layer>>(this, units, data.layers, "Layers", QIcon(":/icons/model-layers.svg"));
    new TreeItem<StringDialog, String>(this, units, data.string, "String", QIcon(":/icons/model-string.svg"));
    new TreeItem<MassesDialog, Masses>(this, units, data.masses, "Masses", QIcon(":/icons/model-masses.svg"));
    new TreeItem<DampingDialog, Damping>(this, units, data.damping, "Damping", QIcon(":/icons/model-damping.svg"));

    QObject::connect(this, &QTreeWidget::itemActivated, [](QTreeWidgetItem* item, int column) {
        auto action = dynamic_cast<Action*>(item);
        if(action != nullptr) {
            action->performAction();
        }
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
