#pragma once
#include "solver/model/input/Layers.hpp"
#include "LayerColors.hpp"
#include <QtWidgets>

class LayerEntry: public QWidget
{
public:
    LayerEntry();
    void setData(const Layer& layer);

private:
    QLabel* symbol;
    QLabel* label;
};

class LayerLegend: public QWidget
{
public:
    LayerLegend();
    void setData(const std::vector<Layer>& layers);

private:
    QVBoxLayout* vbox;
};

