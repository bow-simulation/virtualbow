#pragma once
#include <QTreeWidgetItem>

class PropertyTreeItem: public QTreeWidgetItem {
public:
    PropertyTreeItem(QTreeWidgetItem* parent);

    virtual QVariant data(int column, int role) const override;

    virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    virtual void setEditorData(QWidget* editor, const QModelIndex& index) const;
    virtual void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const;
};
