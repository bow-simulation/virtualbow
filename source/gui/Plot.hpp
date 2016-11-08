#pragma once
#include "../external/qcustomplot/qcustomplot.h"
#include <QtWidgets>

class Series;

class Plot: public QWidget
{
public:
    enum class Align{ BottomLeft, BottomRight, TopLeft, TopRight };

    Plot(const QString& lbx, const QString& lby, Align align = Align::BottomLeft);

    void addSeries();
    void setData(size_t i, const Series& data);
    void setLineStyle(size_t i, QCPCurve::LineStyle style);
    void setScatterStyle(size_t i, QCPScatterStyle style);

    void replot();

    void includeOrigin(bool x, bool y);
    void fixAspectRatio(bool value);

private:
    QCustomPlot* plot;
    QCPAxis* x_axis;
    QCPAxis* y_axis;

    std::vector<QCPCurve*> series;
    bool include_origin_x;
    bool include_origin_y;
    bool fix_aspect_ratio;

    virtual void resizeEvent(QResizeEvent *event) override;
    void copy();
    void exportDialog();
};
