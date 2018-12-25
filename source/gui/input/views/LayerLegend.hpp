#pragma once
#include "bow/input/Layers.hpp"
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
    void setData(const Layers& layers);

private:
    QVBoxLayout* vbox;
};

