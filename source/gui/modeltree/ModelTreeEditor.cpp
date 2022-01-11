#include "ModelTreeEditor.hpp"
#include "ModelTreeItem.hpp"
#include "gui/viewmodel/ViewModel.hpp"
#include "gui/modeltree/comment//CommentDialog.hpp"
#include "gui/modeltree/profile/ProfileDialog.hpp"
#include "gui/modeltree/width/WidthDialog.hpp"
#include "gui/modeltree/layers/LayerDialog.hpp"
#include "gui/modeltree/settings/SettingsDialog.hpp"
#include "gui/modeltree/string/StringDialog.hpp"
#include "gui/modeltree/masses/MassesDialog.hpp"
#include "gui/modeltree/damping/DampingDialog.hpp"
#include "gui/modeltree/dimensions/DimensionsDialog.hpp"

ModelTreeEditor::ModelTreeEditor(ViewModel* model) {
    new ModelTreeItem<CommentDialog, std::string>(this, data.comment, "Comments", QIcon(":/icons/model-comments.svg"));
    new ModelTreeItem<SettingsDialog, Settings>(this, data.settings, "Settings", QIcon(":/icons/model-settings.svg"));
    new ModelTreeItem<DimensionsDialog, Dimensions>(this, data.dimensions, "Dimensions", QIcon(":/icons/model-dimensions.svg"));
    new ModelTreeItem<ProfileDialog, std::vector<SegmentInput>>(this, data.profile, "Profile", QIcon(":/icons/model-profile.svg"));
    new ModelTreeItem<WidthDialog, std::vector<Vector<2>>>(this, data.width, "Width", QIcon(":/icons/model-width.svg"));
    new ModelTreeItem<LayerDialog, std::vector<Layer>>(this, data.layers, "Layers", QIcon(":/icons/model-layers.svg"));
    new ModelTreeItem<StringDialog, String>(this, data.string, "String", QIcon(":/icons/model-string.svg"));
    new ModelTreeItem<MassesDialog, Masses>(this, data.masses, "Masses", QIcon(":/icons/model-masses.svg"));
    new ModelTreeItem<DampingDialog, Damping>(this, data.damping, "Damping", QIcon(":/icons/model-damping.svg"));

    QObject::connect(this, &QTreeWidget::itemActivated, [](QTreeWidgetItem* item, int column) {
        auto action = dynamic_cast<Action*>(item);
        if(action != nullptr) {
            action->performAction();
        }
    });

    this->setHeaderHidden(true);
    this->expandAll();
    this->setItemsExpandable(false);
}

const InputData& ModelTreeEditor::getData() const {
    return data;
}

void ModelTreeEditor::setData(const InputData& data) {
    this->data = data;
}
