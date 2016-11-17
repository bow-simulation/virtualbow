#pragma once
#include "../external/qcustomplot/qcustomplot.h"
#include <QtWidgets>

class Series;

enum class ExpansionMode
{
    None, OneSided, Symmetric
};

class Plot: public QWidget
{
public:
    enum class Align{ BottomLeft, BottomRight, TopLeft, TopRight }; // Todo: Better name than "align"

    Plot(const QString& lbx, const QString& lby, Align align = Align::BottomLeft);

    size_t addSeries();
    size_t addSeries(const Series& data);
    void setData(size_t i, const Series& data);
    void setLineStyle(size_t i, QCPCurve::LineStyle style);
    void setLinePen(size_t i, const QPen& pen);
    void setScatterStyle(size_t i, QCPScatterStyle style);

    void setExpansionMode(ExpansionMode em_x, ExpansionMode em_y);  // Todo: Naming
    void fitContent(bool include_origin_x, bool include_origin_y);
    void replot();
    int count() const;

private:
    QCustomPlot* plot;
    QCPAxis* x_axis;
    QCPAxis* y_axis;

    QCPRange content_range_x;
    QCPRange content_range_y;
    ExpansionMode mode_x;
    ExpansionMode mode_y;

    std::vector<QCPCurve*> series;

    virtual void resizeEvent(QResizeEvent *event) override;
    void copy();
    void exportDialog();
};
