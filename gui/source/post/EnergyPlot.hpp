#pragma once
#include "pre/widgets/PlotWidget.hpp"
#include "pre/viewmodel/units/Quantity.hpp"
#include "solver/model/output2/OutputData2.hpp"

class EnergyPlot: public QWidget {
public:
    EnergyPlot(const StateVec& states, const std::vector<double>& parameter, const QString& label_x, const Quantity& quantity_x, const Quantity& quantity_y);
    void setStateIndex(int index);

private:
    const StateVec& states;
    const std::vector<double>& parameter;
    const Quantity& quantity_x;
    const Quantity& quantity_y;
    QString label_x;

    PlotWidget* plot;

    QCheckBox* cb_stacked;
    QCheckBox* cb_part;
    QCheckBox* cb_type;

    void updatePlot();
};
