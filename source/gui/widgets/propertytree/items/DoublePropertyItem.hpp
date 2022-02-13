#pragma once
#include "gui/viewmodel/properties/DoubleProperty.hpp"
#include "gui/widgets/propertytree/PropertyTreeItem.hpp"
#include "gui/units/UnitSystem.hpp"
#include "gui/utils/DoubleRange.hpp"
#include <functional>

class GroupPropertyItem;

class DoublePropertyItem: public PropertyTreeItem
{
public:
    DoublePropertyItem(const QString& name, const UnitGroup* units, const DoubleRange& range, double step, GroupPropertyItem* parent = nullptr);

    double getValue() const;
    void setValue(double value);

    QVariant data(int column, int role) const override;
    void setData(int column, int role, const QVariant &value) override;

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;

private:
    QString name;
    const UnitGroup* units;
    DoubleRange range;
    double step;
    double value;
};
