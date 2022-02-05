#include "TreeView.hpp"
#include "TreeModel.hpp"
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
        case Types::Materials:
            addMaterial(item->childCount());    // Add new material at the end
            break;

        case Types::Material:
            addMaterial(item_materials->indexOfChild(item) + 1);    // Add new material after selected
            break;

        case Types::Layers:
            addLayer(item->childCount());    // Add new layer at the end
            break;

        case Types::Layer:
            addLayer(item_layers->indexOfChild(item) + 1);    // Add new material after selected
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
        case Types::Materials:
            removeMaterial(item->childCount() - 1);    // Remove material from the end
            break;

        case Types::Material:
            removeMaterial(item_materials->indexOfChild(item));    // Remove selected material
            break;

        case Types::Layers:
            removeLayer(item->childCount() - 1);    // Remove layer from the end
            break;

        case Types::Layer:
            removeLayer(item_layers->indexOfChild(item));    // Remove selected layer
            break;

        case Types::Profile:
            removeSegment(item->childCount() - 1);    // Remove segment from the end
            break;

        case Types::Segment:
            removeSegment(item_profile->indexOfChild(item));    // Remove selected segment
            break;
        }
    });

    button_up = new QToolButton();
    button_up->setIcon(QIcon(":/icons/list-move-up.svg"));
    QObject::connect(button_up, &QToolButton::clicked, this, [&, model] {
        auto item = static_cast<TreeItem*>(tree->currentItem());

        int i = item->parent()->indexOfChild(item);
        if(i == 0) {
            return;    // Move up is not possible for the first item
        }

        switch(item->type()) {
        case Types::Material:
            swapMaterials(i, i-1);
            break;

        case Types::Layer:
            swapLayers(i, i-1);
            break;

        case Types::Segment:
            swapSegments(i, i-1);
            break;
        }
    });

    button_down = new QToolButton();
    button_down->setIcon(QIcon(":/icons/list-move-down.svg"));
    QObject::connect(button_down, &QToolButton::clicked, this, [&, model] {
        auto item = static_cast<TreeItem*>(tree->currentItem());
        int i = item->parent()->indexOfChild(item);

        if(i == item->parent()->childCount() - 1) {
            return;    // Move down is not possible for the last item
        }

        switch(item->type()) {
        case Types::Material:
            swapMaterials(i, i+1);
            break;

        case Types::Layer:
            swapLayers(i, i+1);
            break;

        case Types::Segment:
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
        case Types::Material:
            removeMaterial(item_materials->indexOfChild(item));    // Remove selected material
            break;

        case Types::Layer:
            removeLayer(item_layers->indexOfChild(item));    // Remove selected layer
            break;

        case Types::Segment:
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

    tree->viewport()->setLayout(hbox);
    tree->setHeaderHidden(true);
    tree->setSelectionMode(QAbstractItemView::SingleSelection);
    createTopLevelItems();
    createSegmentMenu();

    // Selection of items
    QObject::connect(tree, &QTreeWidget::currentItemChanged, this, [&](QTreeWidgetItem* current, QTreeWidgetItem* previous){
        auto item = static_cast<TreeItem*>(current);
        updateButtons(item);                             // Update buttons according to selected item
        emit currentEditorChanged(item->getEditor());    // Show editor associated with the item
        emit currentPlotChanged(item->getPlot());        // Show plot associated with the item
    });
}

void TreeView::createSegmentMenu() {
    segment_menu = new QMenu();
    auto add_segment_at_item = [&](const SegmentInput& segment) {
        auto item = static_cast<TreeItem*>(tree->currentItem());
        if(item->type() == Types::Profile) {
            addSegment(item->childCount(), segment);
        }
        else if(item->type() == Types::Segment) {
            addSegment(item_profile->indexOfChild(item) + 1, segment);
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
    item_comments = new TreeItem("Comments", QIcon(":/icons/model-comments.svg"), Types::Comments);
    tree->addTopLevelItem(item_comments);

    item_settings = new TreeItem("Settings", QIcon(":/icons/model-settings.svg"), Types::Settings);
    tree->addTopLevelItem(item_settings);

    item_dimensions = new TreeItem("Dimensions", QIcon(":/icons/model-dimensions.svg"), Types::Dimensions);
    tree->addTopLevelItem(item_dimensions);

    item_materials = new TreeItem("Materials", QIcon(":/icons/model-materials.svg"), Types::Materials);
    tree->addTopLevelItem(item_materials);

    item_layers = new TreeItem("Layers", QIcon(":/icons/model-layers.svg"), Types::Layers);
    tree->addTopLevelItem(item_layers);

    item_profile = new TreeItem("Profile", QIcon(":/icons/model-profile.svg"), Types::Profile);
    tree->addTopLevelItem(item_profile);

    item_width = new TreeItem("Width", QIcon(":/icons/model-width.svg"), Types::Width);
    tree->addTopLevelItem(item_width);

    item_string = new TreeItem("String", QIcon(":/icons/model-string.svg"), Types::String);
    tree->addTopLevelItem(item_string);

    item_masses = new TreeItem("Masses", QIcon(":/icons/model-masses.svg"), Types::Masses);
    tree->addTopLevelItem(item_masses);

    item_damping = new TreeItem("Damping", QIcon(":/icons/model-damping.svg"), Types::Damping);
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
    return new TreeItem(QString::fromStdString(material.name), QIcon(":/icons/model-material.svg"), Types::Material);
}

// TODO: Why "struct tag" here?
QTreeWidgetItem* TreeView::createLayerItem(const struct Layer& layer) const {
    return new TreeItem({QString::fromStdString(layer.name)}, QIcon(":/icons/model-layer.svg"), Types::Layer);
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
    return new TreeItem(segmentText(segment), segmentIcon(segment), Types::Segment);
}

void TreeView::swapTreeItems(QTreeWidgetItem* parent, int i, int j) {
    QTreeWidgetItem* current = tree->currentItem();

    int i_max = std::max(i, j);
    QTreeWidgetItem* item_max = parent->takeChild(i_max);

    int i_min = std::min(i, j);
    QTreeWidgetItem* item_min = parent->takeChild(i_min);

    parent->insertChild(i_min, item_max);
    parent->insertChild(i_max, item_min);

    tree->setCurrentItem(current);
}

void TreeView::updateButtons(TreeItem* selection) {
    bool add_or_remove = selection->type() == Types::Materials || selection->type() == Types::Material ||
                         selection->type() == Types::Layers || selection->type() == Types::Layer ||
                         selection->type() == Types::Profile || selection->type() == Types::Segment;

    bool reorder = selection->type() == Types::Material ||
                   selection->type() == Types::Layer ||
                   selection->type() == Types::Segment;

    button_add->setEnabled(add_or_remove);
    button_remove->setEnabled(add_or_remove);
    button_up->setEnabled(reorder);
    button_down->setEnabled(reorder);

    bool show_segment_menu = selection->type() == Types::Profile || selection->type() == Types::Segment;
    button_add->setMenu(show_segment_menu ? segment_menu : nullptr);
}

void TreeView::addMaterial(int index) {
    auto& material = model->addMaterial(index);
    item_materials->insertChild(index, createMaterialItem(material));
}

void TreeView::removeMaterial(int index) {
    model->removeMaterial(index);
    item_materials->removeChild(item_materials->child(index));
}

void TreeView::swapMaterials(int i, int j) {
    model->swapMaterials(i, j);
    swapTreeItems(item_materials, i, j);
}

void TreeView::addLayer(int index) {
    auto& layer = model->addLayer(index);
    item_layers->insertChild(index, createLayerItem(layer));
}

void TreeView::removeLayer(int index) {
    model->removeLayer(index);
    item_layers->removeChild(item_layers->child(index));
}

void TreeView::swapLayers(int i, int j) {
    model->swapLayers(i, j);
    swapTreeItems(item_layers, i, j);
}


void TreeView::addSegment(int index, const SegmentInput& segment) {
    // TODO: Add in model
    item_profile->insertChild(index, createProfileItem(segment));
}

void TreeView::removeSegment(int index) {
    // TODO: Remove in model
    item_profile->removeChild(item_profile->child(index));
}

void TreeView::swapSegments(int i, int j) {
    // TODO: Swap in model
    swapTreeItems(item_profile, i, j);
}
