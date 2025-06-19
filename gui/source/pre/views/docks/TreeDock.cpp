#include "TreeDock.hpp"
#include "pre/models/MainTreeModel.hpp"
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

TreeDock::TreeDock(MainTreeModel* viewModel, QItemSelectionModel* selectionModel)
    : viewModel(viewModel),
      tree(new QTreeView()),
      menuAddMaterial(createMaterialMenu()),
      menuAddLayer(createLayerMenu()),
      menuAddSegment(createSegmentMenu())
{
    this->setObjectName("PlotView");    // Required to save state of main window
    this->setFeatures(QDockWidget::NoDockWidgetFeatures);
    this->setWindowTitle("Model");
    this->setWidget(tree);

    // Actions that can be triggered by the tool buttons, shortcuts or context menus

    actionRemove = new QAction(QIcon(":/icons/list-remove.svg"), "Delete", tree);
    actionRemove->setShortcut(QKeySequence::Delete);
    actionRemove->setShortcutContext(Qt::WidgetShortcut);
    QObject::connect(actionRemove, &QAction::triggered, this, [=] {
        QModelIndexList selection = selectionModel->selectedIndexes();
        viewModel->removeIndexes(selection);
    });

    actionMoveUp = new QAction(QIcon(":/icons/list-move-up.svg"), "Up", tree);
    QObject::connect(actionMoveUp, &QAction::triggered, this, [=] {
        QModelIndexList selection = selectionModel->selectedIndexes();
        viewModel->moveIndexesUp(selection);
    });

    actionMoveDown = new QAction(QIcon(":/icons/list-move-down.svg"), "Down", tree);
    QObject::connect(actionMoveDown, &QAction::triggered, this, [=] {
        QModelIndexList selection = selectionModel->selectedIndexes();
        viewModel->moveIndexesDown(selection);
    });

    buttonAdd = new QToolButton();
    buttonAdd->setIcon(QIcon(":/icons/list-add.svg"));
    buttonAdd->setPopupMode(QToolButton::InstantPopup);

    auto buttonRemove = new QToolButton();
    buttonRemove->setDefaultAction(actionRemove);

    auto buttonUp = new QToolButton();
    buttonUp->setDefaultAction(actionMoveUp);

    auto buttonDown = new QToolButton();
    buttonDown->setDefaultAction(actionMoveDown);

    auto hbox = new QHBoxLayout();
    hbox->setAlignment(Qt::AlignTop);
    hbox->setContentsMargins(2, 2, 2, 2);
    hbox->setSpacing(2);
    hbox->addStretch();
    hbox->addWidget(buttonAdd);
    hbox->addWidget(buttonRemove);
    hbox->addWidget(buttonUp);
    hbox->addWidget(buttonDown);

    tree->setModel(viewModel);
    tree->setSelectionModel(selectionModel);
    tree->setSelectionMode(QAbstractItemView::ExtendedSelection);
    tree->setContextMenuPolicy(Qt::ActionsContextMenu);
    tree->setLayout(hbox);
    tree->setHeaderHidden(true);

    // Update the button states if either the model data/layout or the item selection changed
    QObject::connect(viewModel, &MainTreeModel::contentModified, this, &TreeDock::updateActions);
    QObject::connect(selectionModel, &QItemSelectionModel::selectionChanged, this, &TreeDock::updateActions);

    updateActions();
}

QMenu* TreeDock::createMaterialMenu() {
    auto menu = new QMenu();
    menu->addAction(QIcon(":/icons/model-material.svg"), "New Material", this, [=]{
        QModelIndex index = tree->selectionModel()->currentIndex();
        if(index.internalId() == ItemType::TOPLEVEL) {
            viewModel->appendMaterial();    // If the top level item is selected, add the new material at the end
        }
        else {
            viewModel->insertMaterial(index.row() + 1);    // If a material is selected, insert the new material below
        }
    });

    return menu;
}

QMenu* TreeDock::createLayerMenu() {
    auto menu = new QMenu();
    menu->addAction(QIcon(":/icons/model-layer.svg"), "New Layer", this, [=]{
        QModelIndex index = tree->selectionModel()->currentIndex();
        if(index.internalId() == ItemType::TOPLEVEL) {
            viewModel->appendLayer();    // If the top level item is selected, add the new layer at the end
        }
        else {
            viewModel->insertLayer(index.row() + 1);    // If a layer is selected, insert the new layer below
        }
    });

    return menu;
}

QMenu* TreeDock::createSegmentMenu() {
    auto add_segment_of_type = [=](SegmentType type) {
        QModelIndex index = tree->selectionModel()->currentIndex();
        if(index.internalId() == ItemType::TOPLEVEL) {
            viewModel->appendSegment(type);    // If the top level item is selected, add the new segment at the end
        }
        else {
            viewModel->insertSegment(index.row() + 1, type);    // If a layer is selected, insert the new segment below
        }
    };

    auto menu = new QMenu();
    menu->addAction(QIcon(":/icons/segment-line.svg"), "New Line", this, [=]{ add_segment_of_type(SegmentType::Line); });
    menu->addAction(QIcon(":/icons/segment-arc.svg"), "New Arc", this, [=]{ add_segment_of_type(SegmentType::Arc); });
    menu->addAction(QIcon(":/icons/segment-spiral.svg"), "New Spiral", this, [=]{ add_segment_of_type(SegmentType::Spiral); });
    menu->addAction(QIcon(":/icons/segment-spline.svg"), "New Spline", this, [=]{ add_segment_of_type(SegmentType::Spline); });
    return menu;
}

// Sets the enabled/disabled state of the buttons as well as the drop down menu of the add button according to
// what can be done with the selected tree items
void TreeDock::updateActions() {
    QModelIndexList selection = tree->selectionModel()->selectedIndexes();

    // Remove all existing actions from the tree view
    for(auto action: tree->actions()) {
        tree->removeAction(action);
    }

    // If adding something is possible, the add button gets assigned the respective menu with the selection of things to add
    // and the tree view gets assigned the same menu actions in order to show them in its context menu in a flattened way.
    if(viewModel->canInsertMaterial(selection)) {
        tree->addActions(menuAddMaterial->actions());
        buttonAdd->setMenu(menuAddMaterial);
        buttonAdd->setEnabled(true);
    }
    else if(viewModel->canInsertLayer(selection)) {
        tree->addActions(menuAddLayer->actions());
        buttonAdd->setMenu(menuAddLayer);
        buttonAdd->setEnabled(true);
    }
    else if(viewModel->canInsertSegment(selection)) {
        tree->addActions(menuAddSegment->actions());
        buttonAdd->setMenu(menuAddSegment);
        buttonAdd->setEnabled(true);
    }
    else {
        buttonAdd->setMenu(nullptr);
        buttonAdd->setEnabled(false);
    }

    // Other actions just get enabled/disbled based on the selection, which also affects all associated buttons and menu items
    actionRemove->setEnabled(viewModel->canRemoveIndexes(selection));
    actionMoveUp->setEnabled(viewModel->canMoveIndexesUp(selection));
    actionMoveDown->setEnabled(viewModel->canMoveIndexesDown(selection));

    // Add back standard actions of the tree view

    auto sep1 = new QAction(tree);
    sep1->setSeparator(true);

    auto sep2 = new QAction(tree);
    sep2->setSeparator(true);

    tree->addAction(sep1);
    tree->addAction(actionMoveUp);
    tree->addAction(actionMoveDown);
    tree->addAction(sep2);
    tree->addAction(actionRemove);
}
