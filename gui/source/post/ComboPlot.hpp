#pragma once
#include "pre/widgets/PlotWidget.hpp"
#include "pre/viewmodel/units/Quantity.hpp"

class ComboPlot: public QWidget {
public:
    ComboPlot();

    void addData(const QString& name, const std::vector<double>& data, const Quantity& quantity);
    void setCombination(int index_x, int index_y);

private:
    QList<const std::vector<double>*> data;
    QList<const Quantity*> quantities;

    QComboBox* combo_x;
    QComboBox* combo_y;
    PlotWidget* plot;
    QCPCurve* curve;

    void updatePlot();
};
