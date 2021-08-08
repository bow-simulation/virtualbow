#pragma once
#include "BaseDialog.hpp"
#include "qcustomplot/qcustomplot.h"
#include <optional>

class PlotWidget;

class BackgroundDialog: public BaseDialog {
    Q_OBJECT

public:
    BackgroundDialog(QWidget* parent, PlotWidget* plot);

    const QPixmap& getPixmap() const;
    double getExtentX() const;
    double getExtentY() const;
    double getOffsetX() const;
    double getOffsetY() const;
    double getAngle() const;

private:
    PlotWidget* plot;    // TODO: Remove

    QPixmap pixmap;

    QLabel* label_extent_x;
    QLabel* label_extent_y;
    QLabel* label_offset_x;
    QLabel* label_offset_y;
    QLabel* label_angle;

    QDoubleSpinBox* spinner_extent_x;
    QDoubleSpinBox* spinner_extent_y;
    QDoubleSpinBox* spinner_offset_x;
    QDoubleSpinBox* spinner_offset_y;
    QDoubleSpinBox* spinner_angle;
    QCheckBox* cb_aspect_ratio;

    void setEditingEnabled(bool enabled);
    void resetTransform();

signals:
    void modified();
};

class PlotWidget: public QCustomPlot {
public:
    enum AspectPolicy {
        NONE, SCALE_X, SCALE_Y
    };

    PlotWidget();
    QMenu* contextMenu();

    void setupTopLegend();
    void rescaleAxes(bool include_zero_x = false, bool include_zero_y = false);
    void setAxesLimits(QCPRange x_range, QCPRange y_range);
    void setAspectPolicy(AspectPolicy policy);

protected:
    virtual QSize sizeHint() const override;
    virtual void resizeEvent(QResizeEvent * event) override;

    void onExport();

private:
    QMenu* menu;
    std::optional<QCPRange> max_x_range;
    std::optional<QCPRange> max_y_range;
    AspectPolicy aspect_policy;

    BackgroundDialog* background_dialog;
    QCPItemPixmap* background;
};
