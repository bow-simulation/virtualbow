#pragma once
#include "gui/widgets/DialogBase.hpp"

class PlotWidget;
class QCPItemPixmap;
class QDoubleSpinBox;
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

    QDoubleSpinBox* spinner_extent_x;
    QDoubleSpinBox* spinner_extent_y;
    QDoubleSpinBox* spinner_offset_x;
    QDoubleSpinBox* spinner_offset_y;
    QDoubleSpinBox* spinner_angle;
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