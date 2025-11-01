#pragma once
#include "solver/BowModel.hpp"
#include <QWidget>

class QLabel;
class QVBoxLayout;

class MaterialEntry: public QWidget
{
public:
    MaterialEntry();
    void setData(const Material& material);

private:
    QLabel* symbol;
    QLabel* label;
};

class MaterialLegend: public QWidget
{
public:
    MaterialLegend();
    void setData(const std::list<Material>& materials);

private:
    QVBoxLayout* vbox;
};
