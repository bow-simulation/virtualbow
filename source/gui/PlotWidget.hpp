#pragma once
#include "qcustomplot/qcustomplot.h"
#include <optional>

class PlotWidget: public QCustomPlot
{
public:
    enum AspectPolicy
    {
        NONE, SCALE_X, SCALE_Y
    };

    PlotWidget(const QSize& size_hint = {600, 400});

    void setupTopLegend();
    void rescaleAxes(bool include_zero_x = false, bool include_zero_y = false);
    void setAxesLimits(QCPRange x_range, QCPRange y_range);
    void setAspectPolicy(AspectPolicy policy);

protected:
    virtual QSize sizeHint() const;
    virtual void resizeEvent(QResizeEvent * event) override;

private:
    QSize size_hint;
    std::optional<QCPRange> max_x_range;
    std::optional<QCPRange> max_y_range;
    AspectPolicy aspect_policy;
};
