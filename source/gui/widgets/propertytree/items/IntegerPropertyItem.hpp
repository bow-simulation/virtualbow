#pragma once
#include "gui/widgets/propertytree/PropertyTreeItem.hpp"
#include "gui/utils/DoubleRange.hpp"

class GroupPropertyItem;

struct IntegerProperty {
    QString name;
    IntegerRange range;
    std::function<double()> get_value;
    std::function<void(double)> set_value;
};

class IntegerPropertyItem: public PropertyTreeItem {
public:
    IntegerPropertyItem(const IntegerProperty& property, GroupPropertyItem* parent = nullptr);

    QVariant data(int column, int role) const override;
    void setData(int column, int role, const QVariant &value) override;

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;

private:
    IntegerProperty property;
};
