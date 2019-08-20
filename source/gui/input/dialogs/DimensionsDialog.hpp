#pragma once
#include "bow/input/Dimensions.hpp"
#include "gui/input/editors/DoubleEditor.hpp"
#include "GroupDialog.hpp"

class DimensionsDialog: public GroupDialog
{
    Q_OBJECT

public:
    DimensionsDialog(QWidget* parent);

    Dimensions getData() const;
    void setData(const Dimensions& data);

signals:
    void modified();

private:
    DoubleEditor* edit0 = new DoubleEditor("Brace height [m]");
    DoubleEditor* edit1 = new DoubleEditor("Draw length [m]");
    DoubleEditor* edit2 = new DoubleEditor("Length [m]");
    DoubleEditor* edit3 = new DoubleEditor("Setback [m]");
    DoubleEditor* edit4 = new DoubleEditor("Angle [rad]");
    DoubleEditor* edit10 = new DoubleEditor("Radius [m]");
};
