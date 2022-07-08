#pragma once
#include "gui/treedock/TreeItem.hpp"
#include "solver/model/input/InputData.hpp"

class ViewModel;
class TableEditor;
class QComboBox;

class LayersTreeItem: public TreeItem
{
public:
    LayersTreeItem(ViewModel* model);
    void initFromModel();
};

class LayerTreeItem: public TreeItem
{
public:
    LayerTreeItem(ViewModel* model);
    Layer getLayer() const;
    void setLayer(const Layer& layer);

protected:
    void setData(int column, int role, const QVariant &value) override;
    void updateCombo();
    void updatePlot();

private:
    TableEditor* table;
    QComboBox* combo;
};
