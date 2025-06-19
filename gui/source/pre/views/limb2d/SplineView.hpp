#pragma once
#include "pre/widgets/PlotWidget.hpp"
#include "pre/models/units/Quantity.hpp"

class SplineView2: public PlotWidget {
public:
    SplineView2(const QString& x_label, const QString& y_label, const Quantity& x_quantity, const Quantity& y_quantity);
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
