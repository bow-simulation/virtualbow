#pragma once
#include "pre/widgets/PlotWidget.hpp"
#include "pre/viewmodel/units/Quantity.hpp"

class SplineView: public PlotWidget {
public:
    SplineView(const QString& x_label, const QString& y_label, const Quantity& x_quantity, const Quantity& y_quantity);
    void setData(const std::vector<std::array<double, 2>>& data);
    void setSelection(const QVector<int>& indices);

private:
    QString x_label;
    QString y_label;
    const Quantity& x_quantity;
    const Quantity& y_quantity;

    std::vector<std::array<double, 2>> input;
    QVector<int> selection;   

    void updatePlot();
    void setNodesVisible(bool visible);
};
