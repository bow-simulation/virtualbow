#pragma once
#include "gui/widgets/propertytree/PropertyTreeItem.hpp"
#include "gui/units/UnitSystem.hpp"

class GroupTreeItem;

class DoubleTreeItem: public PropertyTreeItem {
public:
    DoubleTreeItem(QString name, GroupTreeItem* parent = nullptr);

    QVariant data(int column, int role) const override;
    void setData(int column, int role, const QVariant &value) override;

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;

private:
    QString name;
    double value;
    UnitGroup* units;
};
