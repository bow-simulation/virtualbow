#include "TreeView.hpp"
#include "TreeModel.hpp"
#include "gui/viewmodel/DataViewModel.hpp"
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QToolButton>
#include <QAction>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMenu>
#include <QLabel>

#include <QDebug>

enum ItemType {
    Comments = QTreeWidgetItem::UserType,  // Required by Qt
    Settings,
    Dimensions,
    Materials,
    Material,
    Layers,
    Layer,
    Profile,
    Segment,
    Width,
    String,
    Masses,
    Damping
};

TreeView::TreeView(DataViewModel* model)
    : model(model),
      tree(new QTreeWidget())
{
    this->setObjectName("PlotView");    // Required to save state of main window
    this->setFeatures(QDockWidget::NoDockWidgetFeatures);
    this->setWindowTitle("Model");
    this->setWidget(tree);

    createTopLevelItems();

    // Menu with actions for adding segments
    auto add_menu = new QMenu();
    add_menu->addAction(QIcon(":/icons/segment-line.svg"), "Line", this, [&]{});
    add_menu->addAction(QIcon(":/icons/segment-arc.svg"), "Arc", this, [&]{});
    add_menu->addAction(QIcon(":/icons/segment-spiral.svg"), "Spiral", this, [&]{});
    add_menu->addAction(QIcon(":/icons/segment-spline.svg"), "Spline", this, [&]{});

    auto button_add = new QToolButton();
    button_add->setIcon(QIcon(":/icons/list-add.svg"));
    button_add->setPopupMode(QToolButton::InstantPopup);
    button_add->setMenu(add_menu);

    auto button_remove = new QToolButton();
    button_remove->setIcon(QIcon(":/icons/list-remove.svg"));

    auto button_up = new QToolButton();
    button_up->setIcon(QIcon(":/icons/list-move-up.svg"));

    auto button_down = new QToolButton();
    button_down->setIcon(QIcon(":/icons/list-move-down.svg"));

    auto action_remove = new QAction(this);
    action_remove->setShortcut(QKeySequence::Delete);
    this->addAction(action_remove);

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
    tree->setSelectionMode(QAbstractItemView::ExtendedSelection);

    QObject::connect(tree, &QTreeWidget::itemSelectionChanged, this, [&, model]{
        auto items = tree->selectedItems();
        if(items.size() == 1) {
            switch(items[0]->type()) {
            case ItemType::Comments:
                model->commentsSelected();
                break;

            case ItemType::Settings:
                model->settingsSelected();
                break;

            case ItemType::Dimensions:
                model->dimensionsSelected();
                break;

            case ItemType::Materials:
                model->materialsSelected();
                break;

            case ItemType::Material:
                model->materialsSelected();
                break;

            case ItemType::Layers:
                model->layersSelected();
                break;

            case ItemType::Layer:
                model->layersSelected();
                break;

            case ItemType::Profile:
                model->profileSelected();
                break;

            case ItemType::Segment:
                model->profileSelected();
                break;

            case ItemType::Width:
                model->widthSelected();
                break;

            case ItemType::String:
                model->stringSelected();
                break;

            case ItemType::Masses:
                model->massesSelected();
                break;

            case ItemType::Damping:
                model->dampingSelected();
                break;
            }
        }
        else {
            model->noneSelected();
        }
    });
}

void TreeView::createTopLevelItems() {
    auto item_comments = new QTreeWidgetItem(tree, {"Comments"}, ItemType::Comments);
    item_comments->setIcon(0, QIcon(":/icons/model-comments.svg"));
    tree->addTopLevelItem(item_comments);

    auto item_settings = new QTreeWidgetItem(tree, {"Settings"}, ItemType::Settings);
    item_settings->setIcon(0, QIcon(":/icons/model-settings.svg"));
    tree->addTopLevelItem(item_settings);

    auto item_dimensions = new QTreeWidgetItem(tree, {"Dimensions"}, ItemType::Dimensions);
    item_dimensions->setIcon(0, QIcon(":/icons/model-dimensions.svg"));
    tree->addTopLevelItem(item_dimensions);

    auto item_materials = new QTreeWidgetItem(tree, {"Materials"}, ItemType::Materials);
    item_materials->setIcon(0, QIcon(":/icons/model-materials.svg"));
    tree->addTopLevelItem(item_materials);

    auto item_layers = new QTreeWidgetItem(tree, {"Layers"}, ItemType::Layers);
    item_layers->setIcon(0, QIcon(":/icons/model-layers.svg"));
    tree->addTopLevelItem(item_layers);

    auto item_profile = new QTreeWidgetItem(tree, {"Profile"}, ItemType::Profile);
    item_profile->setIcon(0, QIcon(":/icons/model-profile.svg"));
    tree->addTopLevelItem(item_profile);

    auto item_width = new QTreeWidgetItem(tree, {"Width"}, ItemType::Width);
    item_width->setIcon(0, QIcon(":/icons/model-width.svg"));
    tree->addTopLevelItem(item_width);

    auto item_string = new QTreeWidgetItem(tree, {"String"}, ItemType::String);
    item_string->setIcon(0, QIcon(":/icons/model-string.svg"));
    tree->addTopLevelItem(item_string);

    auto item_masses = new QTreeWidgetItem(tree, {"Masses"}, ItemType::Masses);
    item_masses->setIcon(0, QIcon(":/icons/model-masses.svg"));
    tree->addTopLevelItem(item_masses);

    auto item_damping = new QTreeWidgetItem(tree, {"Damping"}, ItemType::Damping);
    item_damping->setIcon(0, QIcon(":/icons/model-damping.svg"));
    tree->addTopLevelItem(item_damping);

    createMaterialItems(item_materials);
    createProfileItems(item_profile);
    createLayerItems(item_layers);
}

void TreeView::createMaterialItems(QTreeWidgetItem* parent) {
    for(auto& material: model->getData().materials) {
        auto item = new QTreeWidgetItem(parent, {QString::fromStdString(material.name)}, ItemType::Material);
        item->setIcon(0, QIcon(":/icons/model-material.svg"));
    }
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

void TreeView::createLayerItems(QTreeWidgetItem* parent) {
    for(auto& layer: model->getData().layers) {
        auto item = new QTreeWidgetItem(parent, {QString::fromStdString(layer.name)}, ItemType::Layer);
        item->setIcon(0, QIcon(":/icons/model-layer.svg"));
    }
}

void TreeView::createProfileItems(QTreeWidgetItem* parent) {
    for(auto& segment: model->getData().profile) {
        auto item = new QTreeWidgetItem(parent, {segmentText(segment)}, ItemType::Segment);
        item->setIcon(0, segmentIcon(segment));
    }
}
