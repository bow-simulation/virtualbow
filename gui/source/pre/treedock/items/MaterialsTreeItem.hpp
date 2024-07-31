#pragma once
#include "pre/treedock/TreeItem.hpp"
#include "solver/model/input/InputData.hpp"

class ViewModel;
class StringPropertyItem;
class ColorPropertyItem;
class DoublePropertyItem;

class MaterialsTreeItem: public TreeItem
{
public:
    MaterialsTreeItem(ViewModel* model);
    void initFromModel();
};

class MaterialTreeItem: public TreeItem
{
public:
    MaterialTreeItem(ViewModel* model);
    Material getMaterial() const;
    void setMaterial(const Material& material);

protected:
    void setData(int column, int role, const QVariant &value) override;

private:
    ColorPropertyItem* color;
    DoublePropertyItem* rho;
    DoublePropertyItem* E;
};
