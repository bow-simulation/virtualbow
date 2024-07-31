#pragma once
#include "pre/widgets/propertytree/PropertyTreeItem.hpp"
#include "pre/viewmodel/units/UnitSystem.hpp"
#include "pre/utils/DoubleRange.hpp"
#include <functional>

class GroupPropertyItem;

class DoublePropertyItem: public PropertyTreeItem
{
public:
    DoublePropertyItem(const QString& name, const Quantity& quantity, const DoubleRange& range, GroupPropertyItem* parent = nullptr);

    double getValue() const;
    void setValue(double value);

    QVariant data(int column, int role) const override;
    void setData(int column, int role, const QVariant &value) override;

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;

private:
    QString name;
    const Quantity& quantity;
    DoubleRange range;
    double value;
};
