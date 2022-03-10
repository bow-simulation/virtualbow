#pragma once
#include "solver/model/input/InputData.hpp"
#include <QWidget>

class QLabel;
class QVBoxLayout;

class LayerEntry: public QWidget
{
public:
    LayerEntry();
    void setData(const Layer& layer, const std::vector<Material>& materials);

private:
    QLabel* symbol;
    QLabel* label;
};

class LayerLegend: public QWidget
{
public:
    LayerLegend();
    void setData(const std::vector<Layer>& layers, const std::vector<Material>& materials);

private:
    QVBoxLayout* vbox;
};
