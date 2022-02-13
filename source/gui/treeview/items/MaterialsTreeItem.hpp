#pragma once
#include "gui/treeview/TreeItem.hpp"
#include "solver/model/input/InputData.hpp"

class DataViewModel;
class StringPropertyItem;
class ColorPropertyItem;
class DoublePropertyItem;

class MaterialsTreeItem: public TreeItem
{
public:
    MaterialsTreeItem(DataViewModel* model);
    void updateModel();
    void updateView();

    void insertChild(int i, QTreeWidgetItem* item) override;
    void removeChild(int i) override;
    void swapChildren(int i, int j) override;

private:
    DataViewModel* model;
};

class MaterialTreeItem: public TreeItem
{
public:
    MaterialTreeItem(DataViewModel* model, const Material& material);
    Material getMaterial() const;

protected:
    void setData(int column, int role, const QVariant &value) override;
    void updateViewModel();

private:
    ColorPropertyItem* color;
    DoublePropertyItem* rho;
    DoublePropertyItem* E;
};
