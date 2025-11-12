#pragma once
#include "pre/utils/DoubleRange.hpp"
#include <QTableView>
#include <QStyledItemDelegate>

class Quantity;

class TableDelegate: public QStyledItemDelegate
{
    Q_OBJECT

public:
    TableDelegate(const Quantity& quantity, const DoubleRange& range, QObject* parent = nullptr);

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

    void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;

    void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

private:
    const Quantity& quantity;
    DoubleRange range;
};

class TableView: public QTableView {
public:
    TableView();

private:
    void insertAbove();
    void insertBelow();
    void removeRows();
    void cutSelection();
    void copySelection();
    void pasteToSelection();
    void deleteSelection();
};
