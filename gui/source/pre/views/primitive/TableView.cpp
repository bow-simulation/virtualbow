#include "TableView.hpp"
#include "pre/models/units/Quantity.hpp"
#include "pre/widgets/DoubleSpinBox.hpp"
#include <QHeaderView>
#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QMenu>

TableDelegate::TableDelegate(const Quantity& quantity, const DoubleRange& range, QObject* parent):
    QStyledItemDelegate(parent),
    quantity(quantity),
    range(range)
{

}

QWidget* TableDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    auto editor = new DoubleSpinBox(quantity, range, parent);
    editor->showUnit(false);
    editor->setFrame(false);

    // Workaround to update the model on every change to the editor value, not only once when finished
    QObject::connect(editor, &DoubleSpinBox::contentModified, this, [=] {
        QAbstractItemModel* model = const_cast<QAbstractItemModel*>(index.model());
        model->setData(index, editor->value(), Qt::EditRole);
    });

    return editor;
}

void TableDelegate::setEditorData(QWidget* editor, const QModelIndex &index) const {
    // Only write data into the editor if it’s not actively being edited
    if(!editor->hasFocus()) {
        double value = index.model()->data(index, Qt::EditRole).toDouble();
        auto spinner = static_cast<DoubleSpinBox*>(editor);
        spinner->setValue(value);
    }
}

void TableDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const {
    auto spinner = static_cast<DoubleSpinBox*>(editor);
    spinner->interpretText();    // From Qt tutorial
    double value = spinner->value();

    model->setData(index, value, Qt::EditRole);
}

void TableDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    editor->setGeometry(option.rect);
}

TableView::TableView() {
    this->setSelectionMode(QAbstractItemView::ContiguousSelection);
    this->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    this->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    this->verticalHeader()->sectionResizeMode(QHeaderView::Fixed);
    this->verticalHeader()->hide();

    auto actionInsertAbove = new QAction("Insert above", this);
    QObject::connect(actionInsertAbove, &QAction::triggered, this, &TableView::insertAbove);
    this->addAction(actionInsertAbove);

    auto actionInsertBelow = new QAction("Insert below", this);
    QObject::connect(actionInsertBelow, &QAction::triggered, this, &TableView::insertBelow);
    this->addAction(actionInsertBelow);

    auto actionRemoveRow = new QAction("Remove rows", this);
    QObject::connect(actionRemoveRow, &QAction::triggered, this, &TableView::removeRows);
    this->addAction(actionRemoveRow);

    auto actionCut = new QAction("&Cut", this);
    QObject::connect(actionCut, &QAction::triggered, this, &TableView::cutSelection);
    actionCut->setShortcuts(QKeySequence::Cut);
    this->addAction(actionCut);

    auto actionCopy = new QAction("Cop&y", this);
    QObject::connect(actionCopy, &QAction::triggered, this, &TableView::copySelection);
    actionCopy->setShortcuts(QKeySequence::Copy);
    this->addAction(actionCopy);

    auto actionPaste = new QAction("&Paste", this);
    QObject::connect(actionPaste, &QAction::triggered, this, &TableView::pasteToSelection);
    actionPaste->setShortcuts(QKeySequence::Paste);
    this->addAction(actionPaste);

    auto actionDelete = new QAction("&Delete", this);
    QObject::connect(actionDelete, &QAction::triggered, this, &TableView::deleteSelection);
    actionDelete->setShortcut(QKeySequence::Delete);
    actionDelete->setShortcutContext(Qt::WidgetShortcut);
    this->addAction(actionDelete);

    this->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(this, &TableView::customContextMenuRequested, [=](const QPoint& pos) {
        QMenu menu(this);
        menu.addAction(actionInsertAbove);
        menu.addAction(actionInsertBelow);
        menu.addAction(actionRemoveRow);
        menu.addSeparator();
        menu.addAction(actionCut);
        menu.addAction(actionCopy);
        menu.addAction(actionPaste);
        menu.addSeparator();
        menu.addAction(actionDelete);
        menu.exec(this->viewport()->mapToGlobal(pos));
    });
}

void TableView::insertAbove() {
    QModelIndex index = selectedIndexes().at(0);
    model()->insertRow(index.row());
}

void TableView::insertBelow() {
    QModelIndex index = selectedIndexes().at(0);
    model()->insertRow(index.row() + 1);
}

void TableView::removeRows() {
    // Collect all selected rows
    QSet<int> rows;
    for (const QModelIndex& index: selectedIndexes()) {
        rows.insert(index.row());
    }

    // Convert to list and sort descending
    QList<int> rowList = rows.values();
    std::sort(rowList.begin(), rowList.end(), std::greater<int>());

    // Remove rows from bottom to top
    for(int row: rowList) {
        model()->removeRow(row);
    }
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
    QStringList rowContents = text.split("\n", Qt::SkipEmptyParts);

    QModelIndex index = selectedIndexes().at(0);
    int i0 = index.row();
    int j0 = index.column();

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
