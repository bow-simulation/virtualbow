#pragma once
#include "gui/widgets/propertytree/PropertyTreeItem.hpp"
#include "gui/units/UnitSystem.hpp"
#include "gui/utils/DoubleRange.hpp"
#include <functional>

class GroupPropertyItem;

struct DoubleProperty {
    QString name;
    UnitGroup* unit;
    DoubleRange range;
    double stepsize;
    std::function<double()> get_value;
    std::function<void(double)> set_value;
};

class DoublePropertyItem: public PropertyTreeItem {
public:
    DoublePropertyItem(const DoubleProperty& property, GroupPropertyItem* parent = nullptr);

    QVariant data(int column, int role) const override;
    void setData(int column, int role, const QVariant &value) override;

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;

private:
    DoubleProperty property;
};
