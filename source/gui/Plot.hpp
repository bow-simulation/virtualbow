#pragma once
#include "../external/qcustomplot/qcustomplot.h"
#include "../numerics/Series.hpp"
#include <QtWidgets>

class Series;

struct Style
{
    QCPCurve::LineStyle line_style = QCPCurve::lsNone;
    QPen line_pen = Qt::NoPen;
    QBrush line_brush = Qt::NoBrush;
    QCPScatterStyle scatter_style = QCPScatterStyle();

    // Line only
    static Style Line(QColor color, int width = 1);

    // Brush only
    static Style Brush(QColor color);

    // Scatter only
    static Style Scatter(QCPScatterStyle::ScatterShape shape, QColor color, int size = 6);
};

enum class ExpansionMode
{
    None, OneSided, Symmetric
};

class Plot: public QWidget
{
public:
    enum class Align{ BottomLeft, BottomRight, TopLeft, TopRight }; // Todo: Better name than "align"

    Plot(const QString& lbx, const QString& lby, Align align = Align::BottomLeft);

    //size_t addSeries();
    size_t addSeries(const Series& data = Series(), const Style& style = Style::Line(Qt::blue), const QString& name = QString());
    void setData(size_t i, const Series& data);
    void setStyle(size_t i, const Style& style);
    void setName(size_t i, const QString& name);

    void setExpansionMode(ExpansionMode em_x, ExpansionMode em_y);  // Todo: Naming
    void fitContent(bool include_origin_x, bool include_origin_y);
    void setContentRanges(const QCPRange& rx, const QCPRange& ry);
    void replot();

    void showIndicatorX(bool show);
    void showIndicatorY(bool show);
    void setIndicatorX(double x);
    void setIndicatorY(double y);

private:
    QCustomPlot* plot;
    QCPAxis* x_axis;
    QCPAxis* y_axis;

    QCPRange content_range_x;
    QCPRange content_range_y;
    ExpansionMode mode_x;
    ExpansionMode mode_y;

    std::vector<QCPCurve*> series;

    QCPItemStraightLine* indicator_x;
    QCPItemStraightLine* indicator_y;

    virtual void resizeEvent(QResizeEvent *event) override;
    void copy();
    void exportDialog();
};
