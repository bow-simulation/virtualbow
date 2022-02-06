#include "TreeView.hpp"
#include "gui/viewmodel/DataViewModel.hpp"
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QToolButton>
#include <QAction>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMenu>
#include <QLabel>
#include <QScrollBar>

#include <QDebug>

TreeView::TreeView(DataViewModel* model)
    : model(model),
      tree(new QTreeWidget())
{
    this->setObjectName("PlotView");    // Required to save state of main window
    this->setFeatures(QDockWidget::NoDockWidgetFeatures);
    this->setWindowTitle("Model");
    this->setWidget(tree);


    button_add = new QToolButton();
    button_add->setIcon(QIcon(":/icons/list-add.svg"));
    button_add->setPopupMode(QToolButton::InstantPopup);
    QObject::connect(button_add, &QToolButton::clicked, this, [&] {
        auto item = static_cast<TreeItem*>(tree->currentItem());
        switch(item->type()) {
        case TreeItemType::MATERIALS:
            insertMaterial(item->childCount());    // Add new material at the end
            break;

        case TreeItemType::MATERIAL:
            insertMaterial(item_materials->indexOfChild(item) + 1);    // Add new material after selected
            break;

        case TreeItemType::LAYERS:
            insertLayer(item->childCount());    // Add new layer at the end
            break;

        case TreeItemType::LAYER:
            insertLayer(item_layers->indexOfChild(item) + 1);    // Add new material after selected
            break;

            // The case of added profile segments is handled by the context menu of the button,
            // which is enabled if a profile or segment item is selected
        }
    });

    button_remove = new QToolButton();
    button_remove->setIcon(QIcon(":/icons/list-remove.svg"));
    QObject::connect(button_remove, &QToolButton::clicked, this, [&, model] {
        auto item = static_cast<TreeItem*>(tree->currentItem());
        switch(item->type()) {
        case TreeItemType::MATERIALS:
            removeMaterial(item->childCount() - 1);    // Remove material from the end
            break;

        case TreeItemType::MATERIAL:
            removeMaterial(item_materials->indexOfChild(item));    // Remove selected material
            break;

        case TreeItemType::LAYERS:
            removeLayer(item->childCount() - 1);    // Remove layer from the end
            break;

        case TreeItemType::LAYER:
            removeLayer(item_layers->indexOfChild(item));    // Remove selected layer
            break;

        case TreeItemType::PROFILE:
            removeSegment(item->childCount() - 1);    // Remove segment from the end
            break;

        case TreeItemType::SEGMENT:
            removeSegment(item_profile->indexOfChild(item));    // Remove selected segment
            break;
        }
    });

    button_up = new QToolButton();
    button_up->setIcon(QIcon(":/icons/list-move-up.svg"));
    QObject::connect(button_up, &QToolButton::clicked, this, [&, model] {
        auto item = static_cast<TreeItem*>(tree->currentItem());
        int i = item->parent()->indexOfChild(item);

        switch(item->type()) {
        case TreeItemType::MATERIAL:
            swapMaterials(i, i-1);
            break;

        case TreeItemType::LAYER:
            swapLayers(i, i-1);
            break;

        case TreeItemType::SEGMENT:
            swapSegments(i, i-1);
            break;
        }
    });

    button_down = new QToolButton();
    button_down->setIcon(QIcon(":/icons/list-move-down.svg"));
    QObject::connect(button_down, &QToolButton::clicked, this, [&, model] {
        auto item = static_cast<TreeItem*>(tree->currentItem());
        int i = item->parent()->indexOfChild(item);

        switch(item->type()) {
        case TreeItemType::MATERIAL:
            swapMaterials(i, i+1);
            break;

        case TreeItemType::LAYER:
            swapLayers(i, i+1);
            break;

        case TreeItemType::SEGMENT:
            swapSegments(i, i+1);
            break;
        }
    });

    // Key delete action removes selected items, but does nothing if none are selected.
    auto action_remove = new QAction(this);
    action_remove->setShortcut(QKeySequence::Delete);
    this->addAction(action_remove);
    QObject::connect(action_remove, &QAction::triggered, this, [&] {
        auto item = static_cast<TreeItem*>(tree->currentItem());
        switch(item->type()) {
        case TreeItemType::MATERIAL:
            removeMaterial(item_materials->indexOfChild(item));    // Remove selected material
            break;

        case TreeItemType::LAYER:
            removeLayer(item_layers->indexOfChild(item));    // Remove selected layer
            break;

        case TreeItemType::SEGMENT:
            removeSegment(item_profile->indexOfChild(item));    // Remove selected segment
            break;
        }
    });

    auto hbox = new QHBoxLayout();
    hbox->setAlignment(Qt::AlignTop);
    hbox->setMargin(2);
    hbox->setSpacing(2);
    hbox->addStretch();
    hbox->addWidget(button_add);
    hbox->addWidget(button_remove);
    hbox->addWidget(button_up);
    hbox->addWidget(button_down);

    tree->setLayout(hbox);
    //tree->viewport()->setLayout(hbox);
    tree->setHeaderHidden(true);
    tree->setSelectionMode(QAbstractItemView::SingleSelection);
    createTopLevelItems();
    createSegmentMenu();

    // On selection of items
    QObject::connect(tree, &QTreeWidget::currentItemChanged, this, [&](QTreeWidgetItem* current, QTreeWidgetItem* previous){
        auto item = static_cast<TreeItem*>(current);
        emit currentEditorChanged(item->getEditor());    // Show editor associated with the item
        emit currentPlotChanged(item->getPlot());        // Show plot associated with the item
        updateButtons();                                 // Update buttons according to selected item
    });
}

void TreeView::createSegmentMenu() {
    segment_menu = new QMenu();
    auto add_segment_at_item = [&](const SegmentInput& segment) {
        auto item = static_cast<TreeItem*>(tree->currentItem());
        if(item->type() == TreeItemType::PROFILE) {
            insertSegment(item->childCount(), segment);
        }
        else if(item->type() == TreeItemType::SEGMENT) {
            insertSegment(item_profile->indexOfChild(item) + 1, segment);
        }
    };

    segment_menu->addAction(QIcon(":/icons/segment-line.svg"), "Line", this, [=]{ add_segment_at_item(LineInput()); });
    segment_menu->addAction(QIcon(":/icons/segment-arc.svg"), "Arc", this, [=]{ add_segment_at_item(ArcInput()); });
    segment_menu->addAction(QIcon(":/icons/segment-spiral.svg"), "Spiral", this, [=]{ add_segment_at_item(SpiralInput()); });
    segment_menu->addAction(QIcon(":/icons/segment-spline.svg"), "Spline", this, [=]{ add_segment_at_item(SplineInput()); });
}

// TODO: Replace with "rebuildTree" that is triggered when the data model is reloaded
// -> Resets the selection
// -> Resets all editors associated with the items
void TreeView::createTopLevelItems() {
    item_comments = new TreeItem("Comments", QIcon(":/icons/model-comments.svg"), TreeItemType::COMMENTS);
    tree->addTopLevelItem(item_comments);

    item_settings = new TreeItem("Settings", QIcon(":/icons/model-settings.svg"), TreeItemType::SETTINGS);
    tree->addTopLevelItem(item_settings);

    item_dimensions = new TreeItem("Dimensions", QIcon(":/icons/model-dimensions.svg"), TreeItemType::DIMENSIONS);
    tree->addTopLevelItem(item_dimensions);

    item_materials = new TreeItem("Materials", QIcon(":/icons/model-materials.svg"), TreeItemType::MATERIALS);
    tree->addTopLevelItem(item_materials);

    item_layers = new TreeItem("Layers", QIcon(":/icons/model-layers.svg"), TreeItemType::LAYERS);
    tree->addTopLevelItem(item_layers);

    item_profile = new TreeItem("Profile", QIcon(":/icons/model-profile.svg"), TreeItemType::PROFILE);
    tree->addTopLevelItem(item_profile);

    item_width = new TreeItem("Width", QIcon(":/icons/model-width.svg"), TreeItemType::WIDTH);
    tree->addTopLevelItem(item_width);

    item_string = new TreeItem("String", QIcon(":/icons/model-string.svg"), TreeItemType::STRING);
    tree->addTopLevelItem(item_string);

    item_masses = new TreeItem("Masses", QIcon(":/icons/model-masses.svg"), TreeItemType::MASSES);
    tree->addTopLevelItem(item_masses);

    item_damping = new TreeItem("Damping", QIcon(":/icons/model-damping.svg"), TreeItemType::DAMPING);
    tree->addTopLevelItem(item_damping);

    for(auto& material: model->getData().materials) {
        item_materials->addChild(createMaterialItem(material));
    }

    for(auto& layer: model->getData().layers) {
        item_layers->addChild(createLayerItem(layer));
    }

    for(auto& segment: model->getData().profile) {
        item_profile->addChild(createProfileItem(segment));
    }
}

// TODO: Why "struct tag" here?
QTreeWidgetItem* TreeView::createMaterialItem(const struct Material& material) const {
    return new TreeItem(QString::fromStdString(material.name), QIcon(":/icons/model-material.svg"), TreeItemType::MATERIAL);
}

// TODO: Why "struct tag" here?
QTreeWidgetItem* TreeView::createLayerItem(const struct Layer& layer) const {
    return new TreeItem({QString::fromStdString(layer.name)}, QIcon(":/icons/model-layer.svg"), TreeItemType::LAYER);
}

QIcon segmentIcon(const SegmentInput& segment) {
    if(std::holds_alternative<LineInput>(segment)) {
        return QIcon(":/icons/segment-line.svg");
    }
    if(std::holds_alternative<ArcInput>(segment)) {
        return QIcon(":/icons/segment-arc.svg");
    }
    if(std::holds_alternative<SpiralInput>(segment)) {
        return QIcon(":/icons/segment-spiral.svg");
    }
    if(std::holds_alternative<SplineInput>(segment)) {
        return QIcon(":/icons/segment-spline.svg");
    }

    throw std::invalid_argument("Unknown segment type");
}

QString segmentText(const SegmentInput& segment) {
    if(std::holds_alternative<LineInput>(segment)) {
        return "Line";
    }
    if(std::holds_alternative<ArcInput>(segment)) {
        return "Arc";
    }
    if(std::holds_alternative<SpiralInput>(segment)) {
        return "Spiral";
    }
    if(std::holds_alternative<SplineInput>(segment)) {
        return "Spline";
    }

    throw std::invalid_argument("Unknown segment type");
}

QTreeWidgetItem* TreeView::createProfileItem(const SegmentInput& segment) const {
    return new TreeItem(segmentText(segment), segmentIcon(segment), TreeItemType::SEGMENT);
}

// Modifies the given name, if needed, by appending (2), (3), ... (n)
// until it doesn't match any of the item names under parent anymore
QString TreeView::createUniqueName(const QString& name, QTreeWidgetItem* parent) const {
    auto taken = [&](QString name){
        for(int i = 0; i < parent->childCount(); ++i) {
            if(parent->child(i)->text(0) == name) {
                return true;
            }
        }
        return false;
    };

    QString unique = name;
    for(int number = 2; taken(unique); ++number) {
        unique = name + " (" + QString::number(number) + ")";
    }

    return unique;
}

void TreeView::swapTreeItems(QTreeWidgetItem* parent, int i, int j) {
    int i_min = std::min(i, j);
    int i_max = std::max(i, j);

    if(i_min >= 0 && i_max < parent->childCount()) {
        QTreeWidgetItem* current = tree->currentItem();
        QTreeWidgetItem* item_max = parent->takeChild(i_max);
        QTreeWidgetItem* item_min = parent->takeChild(i_min);

        parent->insertChild(i_min, item_max);
        parent->insertChild(i_max, item_min);

        tree->setCurrentItem(current);    // Reset previously selected item
        updateButtons();
    }
}

void TreeView::insertTreeItem(QTreeWidgetItem* parent, QTreeWidgetItem* item, int index) {
    if(index >= 0 && index <= parent->childCount()) {
        parent->insertChild(index, item);
        updateButtons();
    }
}

void TreeView::removeTreeItem(QTreeWidgetItem* parent, int index) {
    if(index >= 0 && index < parent->childCount()) {
        parent->removeChild(parent->child(index));
        updateButtons();
    }
}

void TreeView::updateButtons() {
    QTreeWidgetItem* selection = tree->currentItem();

    bool add_enabled = selection->type() == TreeItemType::MATERIALS || selection->type() == TreeItemType::MATERIAL ||
                       selection->type() == TreeItemType::LAYERS    || selection->type() == TreeItemType::LAYER    ||
                       selection->type() == TreeItemType::PROFILE   || selection->type() == TreeItemType::SEGMENT;

    bool remove_enabled = (selection->type() == TreeItemType::MATERIALS && selection->childCount() > 0) || selection->type() == TreeItemType::MATERIAL ||
                          (selection->type() == TreeItemType::LAYERS && selection->childCount() > 0)    || selection->type() == TreeItemType::LAYER    ||
                          (selection->type() == TreeItemType::PROFILE && selection->childCount() > 9)   || selection->type() == TreeItemType::SEGMENT;

    bool reorder_enabled = selection->type() == TreeItemType::MATERIAL ||
                           selection->type() == TreeItemType::LAYER    ||
                           selection->type() == TreeItemType::SEGMENT;

    button_add->setEnabled(add_enabled);
    button_remove->setEnabled(remove_enabled);
    button_up->setEnabled(reorder_enabled);
    button_down->setEnabled(reorder_enabled);

    bool show_segment_menu = selection->type() == TreeItemType::PROFILE || selection->type() == TreeItemType::SEGMENT;
    button_add->setMenu(show_segment_menu ? segment_menu : nullptr);
}

void TreeView::insertMaterial(int index) {
    QString name = createUniqueName("New material", item_materials);
    Material& material = model->createMaterial(index, name);
    insertTreeItem(item_materials, createMaterialItem(material), index);
}

void TreeView::removeMaterial(int index) {
    model->removeMaterial(index);
    removeTreeItem(item_materials, index);
}

void TreeView::swapMaterials(int i, int j) {
    model->swapMaterials(i, j);
    swapTreeItems(item_materials, i, j);
}

void TreeView::insertLayer(int index) {
    QString name = createUniqueName("New layer", item_layers);
    Layer& layer = model->createLayer(index, name);
    insertTreeItem(item_layers, createLayerItem(layer), index);
}

void TreeView::removeLayer(int index) {
    model->removeLayer(index);
    removeTreeItem(item_layers, index);
}

void TreeView::swapLayers(int i, int j) {
    model->swapLayers(i, j);
    swapTreeItems(item_layers, i, j);
}

void TreeView::insertSegment(int index, const SegmentInput& segment) {
    model->addSegment(index, segment);
    insertTreeItem(item_profile, createProfileItem(segment), index);
}

void TreeView::removeSegment(int index) {
    model->removeSegment(index);
    removeTreeItem(item_profile, index);
}

void TreeView::swapSegments(int i, int j) {
    model->swapSegments(i, j);
    swapTreeItems(item_profile, i, j);
}
