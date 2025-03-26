#include "TreeDock.hpp"
#include "../viewmodels/ModelTreeVM.hpp"
/*
#include "pre/viewmodel/ViewModel.hpp"
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
*/
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QToolButton>
#include <QAction>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMenu>
#include <QLabel>
#include <QScrollBar>
#include <QStringListModel>

#include <QDebug>

TreeDock::TreeDock(ModelTreeVM* viewModel)
    : model(nullptr),
      tree(new QTreeView())//,
      //menu_add_material(createMaterialMenu()),
      //menu_add_layer(createLayerMenu()),
      //menu_add_segment(createSegmentMenu())
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

    button_up = new QToolButton();
    button_up->setIcon(QIcon(":/icons/list-move-up.svg"));

    button_down = new QToolButton();
    button_down->setIcon(QIcon(":/icons/list-move-down.svg"));

    // Key delete action removes selected items, but does nothing if none are selected.
    auto action_remove = new QAction(tree);
    action_remove->setShortcut(QKeySequence::Delete);
    action_remove->setShortcutContext(Qt::WidgetShortcut);
    tree->addAction(action_remove);

    auto hbox = new QHBoxLayout();
    hbox->setAlignment(Qt::AlignTop);
    hbox->setContentsMargins(2, 2, 2, 2);
    hbox->setSpacing(2);
    hbox->addStretch();
    hbox->addWidget(button_add);
    hbox->addWidget(button_remove);
    hbox->addWidget(button_up);
    hbox->addWidget(button_down);

    /*
    QStringListModel *model = new QStringListModel();
    QStringList list;
    list << "a" << "b" << "c";
    model->setStringList(list);
    */

    tree->setLayout(hbox);
    tree->setHeaderHidden(true);
    tree->setSelectionMode(QAbstractItemView::SingleSelection);
    tree->setModel(viewModel);

    /*
    //createTopLevelItems();
    //createSegmentMenu();

    // On selection of a tree item
    QObject::connect(tree, &QTreeWidget::itemSelectionChanged, this, [=](){
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

    createTopLevelItems();
    updateButtons();
    */
}

void TreeDock::createTopLevelItems() {
    /*
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
    */
}

QMenu* TreeDock::createMaterialMenu() {
    /*
    auto menu = new QMenu();
    menu->addAction(QIcon(":/icons/model-material.svg"), "New Material", this, [=]{
        auto item = static_cast<TreeItem*>(tree->currentItem());
        if(item->type() == TreeItemType::MATERIALS) {
            // Category selected: Add new material at the end
            model->insertMaterial(item_materials->childCount(), Material(), this);
        }
        else if(item->type() == TreeItemType::MATERIAL) {
            // Material selected: Add new material after selection
            model->insertMaterial(item_materials->indexOfChild(item) + 1, Material(), this);
        }
    });

    return menu;
    */
}

QMenu* TreeDock::createLayerMenu() {
    /*
    auto menu = new QMenu();
    menu->addAction(QIcon(":/icons/model-layer.svg"), "New Layer", this, [=]{
        auto item = static_cast<TreeItem*>(tree->currentItem());
        if(item->type() == TreeItemType::LAYERS) {
            // Category selected: Add new layer at the end
            model->insertLayer(item_layers->childCount(), Layer(), this);
        }
        else if(item->type() == TreeItemType::LAYER) {
            // Layer selected: Add new layer after selection
            model->insertLayer(item_layers->indexOfChild(item) + 1, Layer(), this);
        }
    });

    return menu;
    */
}

QMenu* TreeDock::createSegmentMenu() {
    //throw std::invalid_argument("Removed code");
    /*
    auto add_segment_at_item = [=](const ProfileSegment& segment) {
        auto item = static_cast<TreeItem*>(tree->currentItem());
        if(item->type() == TreeItemType::PROFILE) {
            model->insertSegment(item_profile->childCount(), segment, this);
        }
        else if(item->type() == TreeItemType::SEGMENT) {
            model->insertSegment(item_profile->indexOfChild(item) + 1, segment, this);
        }
    };
    */

    auto menu = new QMenu();
    /*
    menu->addAction(QIcon(":/icons/segment-line.svg"), "New Line", this, [=]{ add_segment_at_item(Line{{LineConstraint::LENGTH, 0.2}}); });
    menu->addAction(QIcon(":/icons/segment-arc.svg"), "New Arc", this, [=]{ add_segment_at_item(Arc{{ArcConstraint::LENGTH, 0.2}, {ArcConstraint::RADIUS, 0.5}}); });
    menu->addAction(QIcon(":/icons/segment-spiral.svg"), "New Spiral", this, [=]{ add_segment_at_item(Spiral({{SpiralConstraint::LENGTH, 0.2}, {SpiralConstraint::R_START, 0.5}, {SpiralConstraint::R_END, 0.5}})); });
    menu->addAction(QIcon(":/icons/segment-spline.svg"), "New Spline", this, [=]{ add_segment_at_item(SpLine({{0.0, 0.0}, {0.2, 0.0}})); });
    */
    return menu;
}

void TreeDock::updateButtons() {
    /*
    QTreeWidgetItem* selection = tree->currentItem();

    if((selection != nullptr) && (selection->type() == TreeItemType::MATERIALS || selection->type() == TreeItemType::MATERIAL)) {
        button_add->setMenu(menu_add_material);
        button_add->setEnabled(true);
    }
    else if((selection != nullptr) && (selection->type() == TreeItemType::LAYERS || selection->type() == TreeItemType::LAYER)) {
        button_add->setMenu(menu_add_layer);
        button_add->setEnabled(true);
    }
    else if((selection != nullptr) && (selection->type() == TreeItemType::PROFILE || selection->type() == TreeItemType::SEGMENT)) {
        button_add->setMenu(menu_add_segment);
        button_add->setEnabled(true);
    }
    else {
        button_add->setMenu(nullptr);
        button_add->setEnabled(false);
    }

    bool remove_enabled = (selection != nullptr) && ((selection->type() == TreeItemType::MATERIALS && selection->childCount() > 0) || selection->type() == TreeItemType::MATERIAL ||
                                                     (selection->type() == TreeItemType::LAYERS && selection->childCount() > 0)    || selection->type() == TreeItemType::LAYER    ||
                                                     (selection->type() == TreeItemType::PROFILE && selection->childCount() > 0)   || selection->type() == TreeItemType::SEGMENT);

    bool reorder_enabled = (selection != nullptr) && (selection->type() == TreeItemType::MATERIAL ||
                                                      selection->type() == TreeItemType::LAYER    ||
                                                      selection->type() == TreeItemType::SEGMENT);

    button_remove->setEnabled(remove_enabled);
    button_up->setEnabled(reorder_enabled);
    button_down->setEnabled(reorder_enabled);
    */
}
