#include "TreeEditor.hpp"

DialogTreeItem::DialogTreeItem(QTreeWidget* parent, const QString& name, const QIcon& icon, QDialog* dialog)
    : QTreeWidgetItem(parent, {name}),
      dialog(dialog) {
    this->setIcon(0, icon);
}

void DialogTreeItem::showDialog() {
    dialog->show();
}

TreeEditor::TreeEditor() {    
    QObject::connect(dialog_comments, &CommentDialog::modified, this, &TreeEditor::modified);
    new DialogTreeItem(this, "Comments", QIcon(":/icons/model-comments.svg"), dialog_comments);

    QObject::connect(dialog_settings, &SettingsDialog::modified, this, &TreeEditor::modified);
    new DialogTreeItem(this, "Settings", QIcon(":/icons/model-settings.svg"), dialog_settings);

    QObject::connect(dialog_dimensions, &DimensionsDialog::modified, this, &TreeEditor::modified);
    new DialogTreeItem(this, "Dimensions", QIcon(":/icons/model-dimensions.svg"), dialog_dimensions);

    QObject::connect(dialog_profile, &ProfileDialog::modified, this, &TreeEditor::modified);
    new DialogTreeItem(this, "Profile", QIcon(":/icons/model-profile.svg"), dialog_profile);

    QObject::connect(dialog_width, &WidthDialog::modified, this, &TreeEditor::modified);
    new DialogTreeItem(this, "Width", QIcon(":/icons/model-width.svg"), dialog_width);

    dialog_layers = new LayerDialog(this);
    QObject::connect(dialog_layers, &LayerDialog::modified, this, &TreeEditor::modified);
    new DialogTreeItem(this, "Layers", QIcon(":/icons/model-layers.svg"), dialog_layers);

    QObject::connect(dialog_string, &StringDialog::modified, this, &TreeEditor::modified);
    new DialogTreeItem(this, "String", QIcon(":/icons/model-string.svg"), dialog_string);

    QObject::connect(dialog_masses, &MassesDialog::modified, this, &TreeEditor::modified);
    new DialogTreeItem(this, "Masses", QIcon(":/icons/model-masses.svg"), dialog_masses);

    QObject::connect(dialog_damping, &DampingDialog::modified, this, &TreeEditor::modified);
    new DialogTreeItem(this, "Damping", QIcon(":/icons/model-damping.svg"), dialog_damping);

    QObject::connect(this, &QTreeWidget::itemActivated, [=](QTreeWidgetItem* item, int column) {
        auto dialog_item = static_cast<DialogTreeItem*>(item);
        dialog_item->showDialog();
    });

    this->setHeaderLabel("Model");
    this->expandAll();
    this->setItemsExpandable(false);
}

InputData TreeEditor::getData() const {
    InputData data;
    data.comment = dialog_comments->getData();
    data.settings = dialog_settings->getData();
    data.dimensions = dialog_dimensions->getData();
    data.profile = dialog_profile->getData();
    data.width = dialog_width->getData();
    data.layers = dialog_layers->getData();
    data.string = dialog_string->getData();
    data.masses = dialog_masses->getData();
    data.damping = dialog_damping->getData();

    return data;
}

void TreeEditor::setData(const InputData& data) {
    dialog_comments->setData(data.comment);
    dialog_settings->setData(data.settings);
    dialog_dimensions->setData(data.dimensions);
    dialog_profile->setData(data.profile);
    dialog_width->setData(data.width);
    dialog_layers->setData(data.layers);
    dialog_string->setData(data.string);
    dialog_masses->setData(data.masses);
    dialog_damping->setData(data.damping);
}

void TreeEditor::setUnits(const UnitSystem& units) {
    dialog_comments->setUnits(units);
    dialog_settings->setUnits(units);
    dialog_dimensions->setUnits(units);
    dialog_profile->setUnits(units);
    dialog_width->setUnits(units);
    dialog_layers->setUnits(units);
    dialog_string->setUnits(units);
    dialog_masses->setUnits(units);
    dialog_damping->setUnits(units);
}
