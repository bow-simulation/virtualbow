#pragma once
#include "pre/widgets/DialogBase.hpp"

class DoubleSpinBox;
class PlotWidget;
class QCPItemPixmap;
class QCheckBox;
class QGroupBox;

class PlotOverlayDialog: public DialogBase {
    Q_OBJECT

public:
    PlotOverlayDialog(PlotWidget* plot);

private:
    PlotWidget* plot;
    QCPItemPixmap* item;

    QPixmap pixmap;

    DoubleSpinBox* spinner_extent_x;
    DoubleSpinBox* spinner_extent_y;
    DoubleSpinBox* spinner_offset_x;
    DoubleSpinBox* spinner_offset_y;
    DoubleSpinBox* spinner_angle;
    QCheckBox* cb_aspect_ratio;

    QGroupBox* group_image;
    QGroupBox* group_trans;

    void setEditingEnabled(bool enabled);
    void resetTransform();

    void updateAspectRatioX();
    void updateAspectRatioY();
    void updateImage(const QString& file = "");
    void updateItem();
};
