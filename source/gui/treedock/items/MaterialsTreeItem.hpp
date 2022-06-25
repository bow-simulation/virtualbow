#pragma once
#include "gui/treedock/TreeItem.hpp"
#include "solver/model/input/InputData.hpp"

class ViewModel;
class StringPropertyItem;
class ColorPropertyItem;
class DoublePropertyItem;

class MaterialsTreeItem: public TreeItem
{
public:
    MaterialsTreeItem(ViewModel* model);
    void updateModel(void* source) override;
    void updateView(void* source) override;
};

class MaterialTreeItem: public TreeItem
{
public:
    MaterialTreeItem(ViewModel* model, const Material& material);
    Material getMaterial() const;

protected:
    void setData(int column, int role, const QVariant &value) override;

private:
    ColorPropertyItem* color;
    DoublePropertyItem* rho;
    DoublePropertyItem* E;
};
