#pragma once
#include "gui/PlotWidget.hpp"
#include "model/Document.hpp"
#include "numerics/Series.hpp"

class InputData;

class SplineView: public PlotWidget
{
public:
    SplineView(const QString& x_label, const QString& y_label, DocItem<Series>& doc_item);
    void setSelection(const std::vector<int>& indices);

private:
    Connection connection;

    Series input;
    Series output;
    std::vector<int> selection;
    void updatePlot();
};
