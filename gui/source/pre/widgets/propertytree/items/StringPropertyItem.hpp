#pragma once
#include "pre/widgets/propertytree/PropertyTreeItem.hpp"

class GroupPropertyItem;

class StringPropertyItem: public PropertyTreeItem {
public:
    StringPropertyItem(QString name, GroupPropertyItem* parent = nullptr);
    QString getValue() const;
    void setValue(const QString& value);

    QVariant data(int column, int role) const override;
    void setData(int column, int role, const QVariant &value) override;

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;

private:
    QString name;
    QString value;
};
