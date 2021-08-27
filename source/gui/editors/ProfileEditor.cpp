#include "ProfileEditor.hpp"
#include "ProfileTreeModel.hpp"
#include "ProfileTableModel.hpp"
#include "solver/model/ProfileCurve.hpp"
#include <algorithm>

ProfileTreeHeader::ProfileTreeHeader(QWidget* parent, const QList<QToolButton*>& buttons)
    : QHeaderView(Qt::Horizontal, parent)
{
    auto hbox = new QHBoxLayout();
    hbox->setMargin(2);
    hbox->setSpacing(2);
    hbox->addStretch();
    for(auto button: buttons) {
        hbox->addWidget(button);
    }

    this->setLayout(hbox);
};

ProfileEditor::ProfileEditor(const UnitSystem& units) {
    auto button_add = new QToolButton();
    button_add->setIcon(QIcon(":/icons/list-add.svg"));
    button_add->setPopupMode(QToolButton::InstantPopup);
    button_add->setMenu(new QMenu());

    auto button_remove = new QToolButton();
    button_remove->setIcon(QIcon(":/icons/list-remove.svg"));

    auto action_remove = new QAction(this);
    action_remove->setShortcut(QKeySequence::Delete);
    this->addAction(action_remove);

    auto tree_model = new ProfileTreeModel(this);
    auto tree_view = new QTreeView();
    tree_view->setHeader(new ProfileTreeHeader(this, {button_add, button_remove}));
    tree_view->header()->setStretchLastSection(false);
    tree_view->header()->setSectionResizeMode(QHeaderView::Stretch);
    tree_view->header()->setDefaultAlignment(Qt::AlignLeft);
    tree_view->setRootIsDecorated(false);
    tree_view->setSelectionMode(QAbstractItemView::ExtendedSelection);
    tree_view->setDragDropMode(QAbstractItemView::InternalMove);
    tree_view->setDragEnabled(true);
    tree_view->setAcceptDrops(true);
    tree_view->setDropIndicatorShown(true);
    tree_view->setModel(tree_model);

    auto table_model = new ProfileTableModel(this);
    auto table_view = new QTableView();
    table_view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table_view->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    table_view->verticalHeader()->setVisible(false);
    table_view->setModel(table_model);

    auto splitter = new QSplitter(Qt::Vertical);
    splitter->setChildrenCollapsible(false);
    splitter->addWidget(tree_view);
    splitter->addWidget(table_view);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 0);

    auto vbox = new QVBoxLayout();
    vbox->setMargin(0);
    vbox->addWidget(splitter);

    this->setLayout(vbox);

    // Actions for adding and removing segments

    button_add->menu()->addAction(QIcon(":/icons/segment-line.svg"), "Line", this, [tree_view, tree_model] {
        QModelIndexList selection = tree_view->selectionModel()->selectedRows();
        tree_model->insertSegment(selection, SegmentInput{ .type = SegmentType::Line, .constraints = {} });
    });

    button_add->menu()->addAction(QIcon(":/icons/segment-arc.svg"), "Arc", this, [tree_view, tree_model] {
        QModelIndexList selection = tree_view->selectionModel()->selectedRows();
        tree_model->insertSegment(selection, SegmentInput{ .type = SegmentType::Arc, .constraints = {} });
    });

    button_add->menu()->addAction(QIcon(":/icons/segment-spiral.svg"), "Spiral", this, [tree_view, tree_model] {
        QModelIndexList selection = tree_view->selectionModel()->selectedRows();
        tree_model->insertSegment(selection, SegmentInput{ .type = SegmentType::Spiral, .constraints = {} });
    });

    button_add->menu()->addAction(QIcon(":/icons/segment-spline.svg"), "Spline", this, [tree_view, tree_model] {
        QModelIndexList selection = tree_view->selectionModel()->selectedRows();
        tree_model->insertSegment(selection, SegmentInput{ .type = SegmentType::Spline, .constraints = {} });
    });

    QObject::connect(button_remove, &QPushButton::clicked, this, [tree_view, tree_model] {
        QModelIndexList selection = tree_view->selectionModel()->selectedRows();
        tree_model->removeSegments(selection);
    });

    QObject::connect(action_remove, &QAction::triggered, this, [tree_view, tree_model] {
        QModelIndexList selection = tree_view->selectionModel()->selectedRows();
        if(!selection.isEmpty()) {
            tree_model->removeSegments(selection);
        }
    });

    QObject::connect(tree_view->selectionModel(), &QItemSelectionModel::selectionChanged, this, [tree_view](const QItemSelection &selected, const QItemSelection &deselected){
        qInfo() << "Selection changed";
        QModelIndexList selection = tree_view->selectionModel()->selectedRows();
        if(selection.size() == 1) {
            qInfo() << "Single selection: " << selection[0].row();
        }
    });

    /*
    QObject::connect(tree_view, &QTreeView::clicked, this, [](const QModelIndex &index) {
        qInfo() << index.row();
    });
    */

    /*
    QObject::connect(&model, &TableModel::modified, this, &ProfileEditor::modified);
    QObject::connect(this->selectionModel(), &QItemSelectionModel::selectionChanged, this, [&] {
        emit this->rowSelectionChanged(getSelectedIndices());
    });
    */
}

MatrixXd ProfileEditor::getData() const {
    return MatrixXd(); //model.getData();
}

void ProfileEditor::setData(const MatrixXd& data) {
    //model.setData(data);
}

QVector<int> ProfileEditor::getSelectedIndices() {
    //QItemSelectionRange range = selectionModel()->selection().first();
    QVector<int> selection;
    /*
    int index = 0;
    for(int i = 0; i < model.rowCount(); ++i) {
        QVariant arg = model.index(i, 0).data();
        QVariant val = model.index(i, 1).data();
        if(!arg.isNull() && !val.isNull()) {
            if(i >= range.top() && i <= range.bottom()) {
                selection.push_back(index);
            }
            ++index;
        }
    }
    */

    return selection;
}
