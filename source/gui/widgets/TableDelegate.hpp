#pragma once
#include "gui/utils/DoubleRange.hpp"
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
