#pragma once
#include "PlotOverlayDialog.hpp"
#include "extern/qcustomplot/qcustomplot.h"
#include <optional>

class PlotWidget: public QCustomPlot {
public:
    enum AspectPolicy {
        NONE, SCALE_X, SCALE_Y
    };

    PlotWidget();
    QMenu* contextMenu();

    void setupTopLegend();
    void rescaleAxes(bool include_zero_x = false, bool include_zero_y = false, double scale_x = 1.0, double scale_y = 1.0);
    void setAxesLimits(QCPRange x_range, QCPRange y_range);
    void setAspectPolicy(AspectPolicy policy);

protected:
    QSize sizeHint() const override;
    void resizeEvent(QResizeEvent * event) override;

    void onExport();
    bool saveCsv(const QString& path);

private:
    QMenu* menu;
    PlotOverlayDialog* background_dialog;

    std::optional<QCPRange> max_x_range;
    std::optional<QCPRange> max_y_range;
    AspectPolicy aspect_policy;
};
