#pragma once
#include <QStyledItemDelegate>
#include "TableSpinner.hpp"

class TableDelegate: public QStyledItemDelegate
{
    Q_OBJECT

public:
    TableDelegate(const TableSpinnerOptions& options, QObject* parent = nullptr);

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

    void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;

    void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

private:
    TableSpinnerOptions options;
};
