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

TreeDock::TreeDock(ModelTreeVM* viewModel, QItemSelectionModel* selectionModel)
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

    buttonAdd = new QToolButton();
    buttonAdd->setIcon(QIcon(":/icons/list-add.svg"));
    buttonAdd->setPopupMode(QToolButton::InstantPopup);

    buttonRemove = new QToolButton();
    buttonRemove->setIcon(QIcon(":/icons/list-remove.svg"));
    QObject::connect(buttonRemove, &QToolButton::clicked, this, [=] {
        QModelIndexList selection = selectionModel->selectedIndexes();
        viewModel->removeIndexes(selection);
    });

    buttonUp = new QToolButton();
    buttonUp->setIcon(QIcon(":/icons/list-move-up.svg"));
    QObject::connect(buttonUp, &QToolButton::clicked, this, [=] {
        QModelIndexList selection = selectionModel->selectedIndexes();
        viewModel->moveIndexesUp(selection);
    });

    buttonDown = new QToolButton();
    buttonDown->setIcon(QIcon(":/icons/list-move-down.svg"));
    QObject::connect(buttonDown, &QToolButton::clicked, this, [=] {
        QModelIndexList selection = selectionModel->selectedIndexes();
        viewModel->moveIndexesDown(selection);
    });

    /*
    // Key delete action removes selected items, but does nothing if none are selected.
    auto action_remove = new QAction(tree);
    action_remove->setShortcut(QKeySequence::Delete);
    action_remove->setShortcutContext(Qt::WidgetShortcut);
    tree->addAction(action_remove);
    */

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
    tree->setLayout(hbox);
    tree->setHeaderHidden(true);

    // Update the button states if either the model data/layout or the item selection changed
    QObject::connect(viewModel, &ModelTreeVM::modified, this, &TreeDock::updateButtons);
    QObject::connect(selectionModel, &QItemSelectionModel::selectionChanged, this, &TreeDock::updateButtons);

    updateButtons();
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
void TreeDock::updateButtons() {
    QModelIndexList selection = tree->selectionModel()->selectedIndexes();

    if(viewModel->canInsertMaterial(selection)) {
        buttonAdd->setMenu(menuAddMaterial);
        buttonAdd->setEnabled(true);
    }
    else if(viewModel->canInsertLayer(selection)) {
        buttonAdd->setMenu(menuAddLayer);
        buttonAdd->setEnabled(true);
    }
    else if(viewModel->canInsertSegment(selection)) {
        buttonAdd->setMenu(menuAddSegment);
        buttonAdd->setEnabled(true);
    }
    else {
        buttonAdd->setMenu(nullptr);
        buttonAdd->setEnabled(false);
    }

    buttonRemove->setEnabled(viewModel->canRemoveIndexes(selection));
    buttonUp->setEnabled(viewModel->canMoveIndexesUp(selection));
    buttonDown->setEnabled(viewModel->canMoveIndexesDown(selection));
}
