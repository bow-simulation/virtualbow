#pragma once
#include "gui/PlotWidget.hpp"
#include "model/Document.hpp"
#include "numerics/Series.hpp"

class InputData;

class SplineView: public PlotWidget
{
public:
    SplineView(const QString& x_label, const QString& y_label, DocItem<Series>& doc_item);
    void setMarkedControlPoints(const std::vector<int>& indices)
    {
        qInfo() << "Index size:" << indices.size();
    }

private:
    Connection connection;
};
