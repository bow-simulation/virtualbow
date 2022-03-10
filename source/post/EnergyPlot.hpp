#pragma once
#include "gui/widgets/PlotWidget.hpp"
#include "gui/viewmodel/units/UnitSystem.hpp"
#include "solver/model/output/OutputData.hpp"

class EnergyPlot: public QWidget {
public:
    EnergyPlot(const BowStates& states, const std::vector<double>& parameter, const QString& label_x, const UnitGroup& unit_x, const UnitGroup& unit_y);
    void setStateIndex(int index);

private:
    const BowStates& states;
    const std::vector<double>& parameter;
    const UnitGroup& unit_x;
    const UnitGroup& unit_y;
    QString label_x;

    PlotWidget* plot;

    QCheckBox* cb_stacked;
    QCheckBox* cb_part;
    QCheckBox* cb_type;

    void updatePlot();
};
