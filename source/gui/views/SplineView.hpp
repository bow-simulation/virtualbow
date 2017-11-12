#pragma once
#include "gui/PlotWidget.hpp"
#include "model/document/Document.hpp"
#include "numerics/Series.hpp"

class InputData;

class SplineView: public PlotWidget
{
public:
    SplineView(const QString& x_label, const QString& y_label, DocumentItem<Series>& doc_item);
    void setSelection(const std::vector<int>& indices);

private:
    DocumentItem<Series>& doc_item;
    Series input;
    Series output;
    std::vector<int> selection;

    void update();
    void updatePlot();
};
