#pragma once
#include "../external/qcustomplot/qcustomplot.h"
#include "../numerics/Series.hpp"
#include <QtWidgets>

class Series;

struct Style
{
    QCPCurve::LineStyle line_style;
    Qt::GlobalColor line_color;
    int line_width;

     QCPScatterStyle::ScatterShape scatter_style;
    Qt::GlobalColor scatter_color;
    int scatter_size;

    // Line only
    Style(Qt::GlobalColor line_color, int line_width);

    // Scatter only
    Style(QCPScatterStyle::ScatterShape scatter_style, Qt::GlobalColor scatter_color, int scatter_size);
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
    size_t addSeries(const Series& data = Series(), const Style& style = Style(Qt::blue, 1), const QString& name = QString());
    void setData(size_t i, const Series& data);
    void setStyle(size_t i, const Style& style);
    void setName(size_t i, const QString& name);

    void setExpansionMode(ExpansionMode em_x, ExpansionMode em_y);  // Todo: Naming
    void fitContent(bool include_origin_x, bool include_origin_y);
    void setContentRanges(const QCPRange& rx, const QCPRange& ry);
    void replot();

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
