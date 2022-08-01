#include "TableView.hpp"
#include <QHeaderView>
#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QMenu>

TableView::TableView() {
    this->setSelectionMode(QAbstractItemView::ContiguousSelection);
    this->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    this->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    this->verticalHeader()->sectionResizeMode(QHeaderView::Fixed);
    this->verticalHeader()->hide();

    auto action_cut = new QAction("&Cut", this);
    QObject::connect(action_cut, &QAction::triggered, this, &TableView::cutSelection);
    action_cut->setShortcuts(QKeySequence::Cut);
    this->addAction(action_cut);

    auto action_copy = new QAction("Cop&y", this);
    QObject::connect(action_copy, &QAction::triggered, this, &TableView::copySelection);
    action_copy->setShortcuts(QKeySequence::Copy);
    this->addAction(action_copy);

    auto action_paste = new QAction("&Paste", this);
    QObject::connect(action_paste, &QAction::triggered, this, &TableView::pasteToSelection);
    action_paste->setShortcuts(QKeySequence::Paste);
    this->addAction(action_paste);

    auto action_delete = new QAction("&Delete", this);
    QObject::connect(action_delete, &QAction::triggered, this, &TableView::deleteSelection);
    action_delete->setShortcut(QKeySequence::Delete);
    action_delete->setShortcutContext(Qt::WidgetShortcut);
    this->addAction(action_delete);

    this->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(this, &TableView::customContextMenuRequested, [=](const QPoint& pos){
        QMenu menu(this);
        menu.addAction(action_cut);
        menu.addAction(action_copy);
        menu.addAction(action_paste);
        menu.addSeparator();
        menu.addAction(action_delete);
        menu.exec(this->viewport()->mapToGlobal(pos));
    });
}

void TableView::cutSelection() {
    copySelection();
    deleteSelection();
}

// https://www.walletfox.com/course/qtableviewcopypaste.php
void TableView::copySelection() {
    QString text;
    QItemSelectionRange range = selectionModel()->selection().first();
    for(int i = range.top(); i <= range.bottom(); ++i) {
        QStringList rowContent;
        for(auto j = range.left(); j <= range.right(); ++j) {
            rowContent.append(model()->index(i, j).data(Qt::DisplayRole).toString());
        }
        text += rowContent.join("\t") + "\n";
    }
    QApplication::clipboard()->setText(text);
}

// https://www.walletfox.com/course/qtableviewcopypaste.php
void TableView::pasteToSelection() {
    QString text = QApplication::clipboard()->text();
    QStringList rowContents = text.split("\n", QString::SkipEmptyParts);

    QModelIndex initIndex = selectedIndexes().at(0);
    int i0 = initIndex.row();
    int j0 = initIndex.column();

    for(int i = 0; i < rowContents.size(); ++i) {
        QStringList columnContents = rowContents.at(i).split("\t");
        for(int j = 0; j < columnContents.size(); ++j) {
            model()->setData(model()->index(i0 + i, j0 + j), columnContents.at(j), Qt::DisplayRole);
        }
    }
}

// https://www.walletfox.com/course/qtableviewcopypaste.php
void TableView::deleteSelection() {
    for(QModelIndex index: selectedIndexes()) {
        model()->setData(index, QVariant());
    }
}
