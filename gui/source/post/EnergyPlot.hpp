#pragma once
#include "pre/widgets/PlotWidget.hpp"
#include "pre/viewmodel/units/UnitSystem.hpp"
#include "solver/model/output/OutputData.hpp"

class EnergyPlot: public QWidget {
public:
    EnergyPlot(const BowStates& states, const std::vector<double>& parameter, const QString& label_x, const Quantity& quantity_x, const Quantity& quantity_y);
    void setStateIndex(int index);

private:
    const BowStates& states;
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
