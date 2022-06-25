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
    void updateModel(void* source) override;
    void updateView(void* source) override;
};

class LayerTreeItem: public TreeItem
{
public:
    LayerTreeItem(ViewModel* model, const Layer& layer);
    Layer getLayer() const;

protected:
    void setData(int column, int role, const QVariant &value) override;
    void updateCombo();
    void updatePlot();

private:
    TableEditor* table;
    QComboBox* combo;
};
