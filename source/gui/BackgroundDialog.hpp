#pragma once
#include "BaseDialog.hpp"

class PlotWidget;
class QCPItemPixmap;

class BackgroundDialog: public BaseDialog {
    Q_OBJECT

public:
    BackgroundDialog(PlotWidget* plot);

private:
    PlotWidget* plot;
    QCPItemPixmap* item;

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

    void updateAspectRatioX();
    void updateAspectRatioY();
    void updateImage(const QString& file = "");
    void updateItem();
};
