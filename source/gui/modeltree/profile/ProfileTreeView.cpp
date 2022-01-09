#include "ProfileTreeView.hpp"
#include "ProfileTreeModel.hpp"
#include "segments/LineSegmentEditor.hpp"
#include "segments/ArcSegmentEditor.hpp"
#include "segments/SpiralSegmentEditor.hpp"
#include "segments/SplineSegmentEditor.hpp"
#include <QHBoxLayout>
#include <QToolButton>
#include <QPushButton>
#include <QTableWidget>
#include <QAction>
#include <QMenu>

#include <QDebug>

// Custom header view that displays some buttons on the top right corner
ProfileTreeHeader::ProfileTreeHeader(QWidget* parent, const QList<QToolButton*>& buttons)
    : QHeaderView(Qt::Horizontal, parent)
{
    auto hbox = new QHBoxLayout();
    this->setLayout(hbox);
    hbox->setMargin(2);
    hbox->setSpacing(2);
    hbox->addStretch();

    for(auto button: buttons) {
        hbox->addWidget(button);
    }
}

ProfileTreeView::ProfileTreeView(ProfileTreeModel* model)
    : model(model)
{
    this->setModel(model);

    // Menu with actions for adding segments
    auto add_menu = new QMenu();
    add_menu->addAction(QIcon(":/icons/segment-line.svg"), "Line", this, [&] { onButtonAdd(LineInput()); });
    add_menu->addAction(QIcon(":/icons/segment-arc.svg"), "Arc", this, [&] { onButtonAdd(ArcInput()); });
    add_menu->addAction(QIcon(":/icons/segment-spiral.svg"), "Spiral", this, [&] { onButtonAdd(SpiralInput()); });
    add_menu->addAction(QIcon(":/icons/segment-spline.svg"), "Spline", this, [&] { onButtonAdd(SplineInput()); });

    button_add = new QToolButton();
    button_add->setIcon(QIcon(":/icons/list-add.svg"));
    button_add->setPopupMode(QToolButton::InstantPopup);
    button_add->setMenu(add_menu);

    button_remove = new QToolButton();
    button_remove->setIcon(QIcon(":/icons/list-remove.svg"));

    button_up = new QToolButton();
    button_up->setIcon(QIcon(":/icons/list-move-up.svg"));

    button_down = new QToolButton();
    button_down->setIcon(QIcon(":/icons/list-move-down.svg"));

    auto action_remove = new QAction(this);
    action_remove->setShortcut(QKeySequence::Delete);
    this->addAction(action_remove);

    // The remove button removes selected segments or the last segment if none are selected.
    QObject::connect(button_remove, &QPushButton::clicked, this, [=] {
        auto selection = this->selectionModel()->selectedIndexes();
        if(selection.empty()) {
            model->removeSegment();
        }
        else {
            model->removeSegments(selection);
        }
    });

    // Key delete action removes selected segments, but does nothing if none are selected.
    QObject::connect(action_remove, &QAction::triggered, this, [=] {
        auto selection = this->selectionModel()->selectedIndexes();
        if(!selection.empty()) {
            model->removeSegments(selection);
        }
    });

    QObject::connect(button_up, &QPushButton::clicked, this, [=] {
        model->moveSegmentsUp(this->selectionModel()->selectedIndexes());
    });

    QObject::connect(button_down, &QPushButton::clicked, this, [=] {
        model->moveSegmentsDown(this->selectionModel()->selectedIndexes());
    });

    // Update button states on selection and data changes
    QObject::connect(model, &ProfileTreeModel::modified, this, &ProfileTreeView::updateButtons);
    QObject::connect(this->selectionModel(), &QItemSelectionModel::selectionChanged, this, &ProfileTreeView::updateButtons);
    updateButtons();

    auto header = new ProfileTreeHeader(this, {button_add, button_remove, button_up, button_down});
    header->setStretchLastSection(false);
    header->setSectionResizeMode(QHeaderView::Stretch);
    header->setDefaultAlignment(Qt::AlignLeft);

    this->setHeader(header);
    this->setRootIsDecorated(false);
    this->setSelectionMode(QAbstractItemView::ExtendedSelection);
    this->setDragDropMode(QAbstractItemView::InternalMove);
    this->setDragEnabled(true);
    this->setAcceptDrops(true);
    this->setDropIndicatorShown(true);
}

// Adds a new segment with the given input
// * If no segment is selected, append new segment at the end
// * If one segment is selected, insert before selection
void ProfileTreeView::onButtonAdd(const SegmentInput& input) {
    auto selection = this->selectionModel()->selectedIndexes();
    if(selection.empty()) {
        model->appendSegment(input);
    }
    else if(selection.size() == 1) {
        model->insertSegment(selection[0], input);
    }
}

// Enables/disables buttons based on the current selection and data
// * Add is possible if none or exactly one item is selected
// * Remove is possible if the list contains at least one item
// * Move up is possible if one or more items are selected, except the first one
// * Move down is possible if one or more items are selected, except the last one
void ProfileTreeView::updateButtons() {
    auto selection = this->selectionModel()->selectedIndexes();
    bool first_selected = this->selectionModel()->isRowSelected(0);
    bool last_selected = this->selectionModel()->isRowSelected(model->rowCount()-1);

    button_add->setEnabled(selection.size() <= 1);
    button_remove->setEnabled(model->rowCount() > 0);
    button_up->setEnabled(!selection.empty() && !first_selected);
    button_down->setEnabled(!selection.empty() && !last_selected);
}
