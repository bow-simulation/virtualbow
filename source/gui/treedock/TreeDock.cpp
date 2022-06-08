#include "TreeDock.hpp"
#include "gui/viewmodel/DataViewModel.hpp"
#include "items/CommentTreeItem.hpp"
#include "items/SettingsTreeItem.hpp"
#include "items/DimensionsTreeItem.hpp"
#include "items/MaterialsTreeItem.hpp"
#include "items/StringTreeItem.hpp"
#include "items/MassesTreeItem.hpp"
#include "items/DampingTreeItem.hpp"
#include "items/WidthTreeItem.hpp"
#include "items/LayersTreeItem.hpp"
#include "items/ProfileTreeItem.hpp"
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

TreeDock::TreeDock(DataViewModel* model)
    : model(model),
      tree(new QTreeWidget()),
      menu_add_material(createMaterialMenu()),
      menu_add_layer(createLayerMenu()),
      menu_add_segment(createSegmentMenu())
{
    this->setObjectName("PlotView");    // Required to save state of main window
    this->setFeatures(QDockWidget::NoDockWidgetFeatures);
    this->setWindowTitle("Model");
    this->setWidget(tree);

    button_add = new QToolButton();
    button_add->setIcon(QIcon(":/icons/list-add.svg"));
    button_add->setPopupMode(QToolButton::InstantPopup);

    button_remove = new QToolButton();
    button_remove->setIcon(QIcon(":/icons/list-remove.svg"));
    QObject::connect(button_remove, &QToolButton::clicked, this, [&] {
        auto selected = static_cast<TreeItem*>(tree->currentItem());
        auto parent = static_cast<TreeItem*>(selected->parent());

        if(parent != nullptr) {
            // Remove selected item from parent
            parent->removeChild(parent->indexOfChild(selected));
        }
        else {
            // Remove last item from selected
            selected->removeChild(selected->childCount() - 1);
        }
    });

    button_up = new QToolButton();
    button_up->setIcon(QIcon(":/icons/list-move-up.svg"));
    QObject::connect(button_up, &QToolButton::clicked, this, [&] {
        auto selected = static_cast<TreeItem*>(tree->currentItem());
        auto parent = static_cast<TreeItem*>(selected->parent());

        if(parent != nullptr) {
            int i = parent->indexOfChild(selected);
            parent->swapChildren(i, i-1);
        }
    });

    button_down = new QToolButton();
    button_down->setIcon(QIcon(":/icons/list-move-down.svg"));
    QObject::connect(button_down, &QToolButton::clicked, this, [&] {
        auto selected = static_cast<TreeItem*>(tree->currentItem());
        auto parent = static_cast<TreeItem*>(selected->parent());

        if(parent != nullptr) {
            int i = parent->indexOfChild(selected);
            parent->swapChildren(i, i+1);
        }
    });

    /*
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
    */

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
    tree->setHeaderHidden(true);
    tree->setSelectionMode(QAbstractItemView::SingleSelection);

    //createTopLevelItems();
    //createSegmentMenu();

    // On selection of a tree item
    QObject::connect(tree, &QTreeWidget::itemSelectionChanged, this, [&](){
        auto items = tree->selectedItems();
        if(items.size() == 1) {
            auto item = static_cast<TreeItem*>(items[0]);
            auto edit_item = item;
            auto plot_item = item;

            // Move up the tree from selected item until an editor != nullptr is found
            while(edit_item->getEditor() == nullptr && edit_item->parent() != nullptr) {
                edit_item = static_cast<TreeItem*>(edit_item->parent());
            }

            // Move up the tree from selected item until a plot != nullptr is found
            while(plot_item->getPlot() == nullptr && plot_item->parent() != nullptr) {
                plot_item = static_cast<TreeItem*>(plot_item->parent());
            }

            // Show editor and plot associated with the items
            emit currentEditorChanged(edit_item->getEditor());
            emit currentPlotChanged(plot_item->getPlot());
        }
        else {
            emit currentEditorChanged(nullptr);
            emit currentPlotChanged(nullptr);
        }
        updateButtons();    // Update buttons according to selection
    });

    // (Re)build tree when data is loaded (resets all selections and editors)
    QObject::connect(model, &DataViewModel::reloaded, this, &TreeDock::rebuildTree);
}

void TreeDock::rebuildTree() {
    // Remove existing items
    tree->clear();

    // Add new items
    item_comments = new CommentTreeItem(model);
    tree->addTopLevelItem(item_comments);

    item_settings = new SettingsTreeItem(model);
    tree->addTopLevelItem(item_settings);

    item_dimensions = new DimensionsTreeItem(model);
    tree->addTopLevelItem(item_dimensions);

    item_materials = new MaterialsTreeItem(model);
    tree->addTopLevelItem(item_materials);

    item_layers = new LayersTreeItem(model);
    tree->addTopLevelItem(item_layers);

    item_profile = new ProfileTreeItem(model);
    tree->addTopLevelItem(item_profile);

    item_width = new WidthTreeItem(model);
    tree->addTopLevelItem(item_width);

    item_string = new StringTreeItem(model);
    tree->addTopLevelItem(item_string);

    item_masses = new MassesTreeItem(model);
    tree->addTopLevelItem(item_masses);

    item_damping = new DampingTreeItem(model);
    tree->addTopLevelItem(item_damping);

    // TODO?
    // updateButtons();
}

QMenu* TreeDock::createMaterialMenu() {
    auto menu = new QMenu();
    menu->addAction(QIcon(":/icons/model-material.svg"), "New Material", this, [&]{
        auto item = static_cast<TreeItem*>(tree->currentItem());
        if(item->type() == TreeItemType::MATERIALS) {
            // Category selected: Add new material at the end
            item_materials->insertChild(item_materials->childCount(), new MaterialTreeItem(model, Material()));
        }
        else if(item->type() == TreeItemType::MATERIAL) {
            // Material selected: Add new material after selection
            item_materials->insertChild(item_materials->indexOfChild(item) + 1, new MaterialTreeItem(model, Material()));
        }
    });

    return menu;
}

QMenu* TreeDock::createLayerMenu() {
    auto menu = new QMenu();
    menu->addAction(QIcon(":/icons/model-layer.svg"), "New Layer", this, [=]{
        auto item = static_cast<TreeItem*>(tree->currentItem());
        if(item->type() == TreeItemType::LAYERS) {
            // Category selected: Add new layer at the end
            item_layers->insertChild(item_layers->childCount(), new LayerTreeItem(model, Layer()));
        }
        else if(item->type() == TreeItemType::LAYER) {
            // Layer selected: Add new layer after selection
            item_layers->insertChild(item_layers->indexOfChild(item) + 1, new LayerTreeItem(model, Layer()));
        }
    });

    return menu;
}

QMenu* TreeDock::createSegmentMenu() {
    auto add_segment_at_item = [&](const SegmentInput& segment) {
        auto item = static_cast<TreeItem*>(tree->currentItem());
        if(item->type() == TreeItemType::PROFILE) {
            item_profile->insertChild(item_profile->childCount(), new SegmentTreeItem(segment));
        }
        else if(item->type() == TreeItemType::SEGMENT) {
            item_profile->insertChild(item_profile->indexOfChild(item) + 1, new SegmentTreeItem(segment));
        }
    };

    auto menu = new QMenu();
    menu->addAction(QIcon(":/icons/segment-line.svg"), "New Line", this, [=]{ add_segment_at_item(LineInput{{LineConstraint::LENGTH, 0.2}}); });
    menu->addAction(QIcon(":/icons/segment-arc.svg"), "New Arc", this, [=]{ add_segment_at_item(ArcInput{{ArcConstraint::LENGTH, 0.2}, {ArcConstraint::R_START, 0.5}}); });
    menu->addAction(QIcon(":/icons/segment-spiral.svg"), "New Spiral", this, [=]{ add_segment_at_item(SpiralInput({{SpiralConstraint::LENGTH, 0.2}, {SpiralConstraint::R_START, 0.5}, {SpiralConstraint::R_END, 0.5}})); });
    menu->addAction(QIcon(":/icons/segment-spline.svg"), "New Spline", this, [=]{ add_segment_at_item(SplineInput({{0.0, 0.0}, {0.2, 0.0}})); });

    return menu;
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

QTreeWidgetItem* TreeDock::createProfileItem(const SegmentInput& segment) const {
    return new TreeItem(segmentText(segment), segmentIcon(segment), TreeItemType::SEGMENT);
}

// Modifies the given name, if needed, by appending (2), (3), ... (n)
// until it doesn't match any of the item names under parent anymore
QString TreeDock::createUniqueName(const QString& name, QTreeWidgetItem* parent) const {
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

void TreeDock::updateButtons() {
    QTreeWidgetItem* selection = tree->currentItem();

    if(selection->type() == TreeItemType::MATERIALS || selection->type() == TreeItemType::MATERIAL) {
        button_add->setMenu(menu_add_material);
        button_add->setEnabled(true);
    }
    else if (selection->type() == TreeItemType::LAYERS || selection->type() == TreeItemType::LAYER) {
        button_add->setMenu(menu_add_layer);
        button_add->setEnabled(true);
    }
    else if (selection->type() == TreeItemType::PROFILE || selection->type() == TreeItemType::SEGMENT) {
        button_add->setMenu(menu_add_segment);
        button_add->setEnabled(true);
    }
    else {
        button_add->setMenu(nullptr);
        button_add->setEnabled(false);
    }

    bool remove_enabled = (selection->type() == TreeItemType::MATERIALS && selection->childCount() > 0) || selection->type() == TreeItemType::MATERIAL ||
                          (selection->type() == TreeItemType::LAYERS && selection->childCount() > 0)    || selection->type() == TreeItemType::LAYER    ||
                          (selection->type() == TreeItemType::PROFILE && selection->childCount() > 0)   || selection->type() == TreeItemType::SEGMENT;

    bool reorder_enabled = selection->type() == TreeItemType::MATERIAL ||
                           selection->type() == TreeItemType::LAYER    ||
                           selection->type() == TreeItemType::SEGMENT;

    button_remove->setEnabled(remove_enabled);
    button_up->setEnabled(reorder_enabled);
    button_down->setEnabled(reorder_enabled);
}
