#pragma once
#include "gui/treeview/TreeItem.hpp"
#include "solver/model/input/InputData.hpp"

class DataViewModel;
class TableEditor;
class QComboBox;

class LayersTreeItem: public TreeItem
{
public:
    LayersTreeItem(DataViewModel* model);
    void updateModel();
    void updateView();

    void insertChild(int i, QTreeWidgetItem* item) override;
    void removeChild(int i) override;
    void swapChildren(int i, int j) override;

private:
    DataViewModel* model;
};

class LayerTreeItem: public TreeItem
{
public:
    LayerTreeItem(DataViewModel* model, const Layer& layer);
    Layer getLayer() const;

protected:
    void setData(int column, int role, const QVariant &value) override;
    void updateModel();
    void updateCombo();

private:
    DataViewModel* model;
    TableEditor* table;
    QComboBox* combo;
};
