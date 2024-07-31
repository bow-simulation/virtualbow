#pragma once
#include "pre/widgets/propertytree/PropertyTreeItem.hpp"
#include "pre/utils/IntegerRange.hpp"

class GroupPropertyItem;

class IntegerPropertyItem: public PropertyTreeItem {
public:
    IntegerPropertyItem(const QString& name, const IntegerRange& range, GroupPropertyItem* parent = nullptr);
    int getValue() const;
    void setValue(int value);

    QVariant data(int column, int role) const override;
    void setData(int column, int role, const QVariant &value) override;

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;

private:
    QString name;
    IntegerRange range;
    int value;
};
