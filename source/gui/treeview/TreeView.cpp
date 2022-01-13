#include "TreeView.hpp"
#include "TreeModel.hpp"
#include "gui/viewmodel/ViewModel.hpp"
#include <QTreeWidgetItem>
#include <QToolButton>
#include <QAction>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMenu>
#include <QLabel>

TreeView::TreeView(ViewModel* model) {
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

    this->viewport()->setLayout(hbox);
    this->setHeaderHidden(true);
}

void TreeView::createTopLevelItems() {
    auto item1 = new QTreeWidgetItem(this, {"Comments"});
    item1->setIcon(0, QIcon(":/icons/model-comments.svg"));
    addTopLevelItem(item1);

    auto item2 = new QTreeWidgetItem(this, {"Settings"});
    item2->setIcon(0, QIcon(":/icons/model-settings.svg"));
    addTopLevelItem(item2);

    auto item4 = new QTreeWidgetItem(this, {"Dimensions"});
    item4->setIcon(0, QIcon(":/icons/model-dimensions.svg"));
    addTopLevelItem(item4);

    auto item3 = new QTreeWidgetItem(this, {"Materials"});
    item3->setIcon(0, QIcon(":/icons/model-materials.svg"));
    addTopLevelItem(item3);

    auto item5 = new QTreeWidgetItem(this, {"Profile"});
    item5->setIcon(0, QIcon(":/icons/model-profile.svg"));
    addTopLevelItem(item5);

    auto item7 = new QTreeWidgetItem(this, {"Layers"});
    item7->setIcon(0, QIcon(":/icons/model-layers.svg"));
    addTopLevelItem(item7);

    auto item6 = new QTreeWidgetItem(this, {"Width"});
    item6->setIcon(0, QIcon(":/icons/model-width.svg"));
    addTopLevelItem(item6);

    auto item8 = new QTreeWidgetItem(this, {"String"});
    item8->setIcon(0, QIcon(":/icons/model-string.svg"));
    addTopLevelItem(item8);

    auto item9 = new QTreeWidgetItem(this, {"Masses"});
    item9->setIcon(0, QIcon(":/icons/model-masses.svg"));
    addTopLevelItem(item9);

    auto item10 = new QTreeWidgetItem(this, {"Damping"});
    item10->setIcon(0, QIcon(":/icons/model-damping.svg"));
    addTopLevelItem(item10);

    createMaterialItems(item3);
    createProfileItems(item5);
    createLayerItems(item7);
}

void TreeView::createMaterialItems(QTreeWidgetItem* parent) {
    auto item1 = new QTreeWidgetItem(parent, {"Ash"});
    item1->setIcon(0, QIcon(":/icons/model-material.svg"));

    auto item2 = new QTreeWidgetItem(parent, {"Elm"});
    item2->setIcon(0, QIcon(":/icons/model-material.svg"));

    auto item3 = new QTreeWidgetItem(parent, {"Yew"});
    item3->setIcon(0, QIcon(":/icons/model-material.svg"));
}

void TreeView::createProfileItems(QTreeWidgetItem* parent) {
    auto item1 = new QTreeWidgetItem(parent, {"Line"});
    item1->setIcon(0, QIcon(":/icons/segment-line.svg"));

    auto item2 = new QTreeWidgetItem(parent, {"Arc"});
    item2->setIcon(0, QIcon(":/icons/segment-arc.svg"));

    auto item3 = new QTreeWidgetItem(parent, {"Spiral"});
    item3->setIcon(0, QIcon(":/icons/segment-spiral.svg"));
}

void TreeView::createLayerItems(QTreeWidgetItem* parent) {
    auto item1 = new QTreeWidgetItem(parent, {"Layer 1"});
    item1->setIcon(0, QIcon(":/icons/model-layer.svg"));

    auto item2 = new QTreeWidgetItem(parent, {"Layer 2"});
    item2->setIcon(0, QIcon(":/icons/model-layer.svg"));

    auto item3 = new QTreeWidgetItem(parent, {"Layer 3"});
    item3->setIcon(0, QIcon(":/icons/model-layer.svg"));
}
