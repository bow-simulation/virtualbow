#pragma once
#include "solver/model//input/Dimensions.hpp"
#include "gui/editors/DoubleEditor.hpp"
#include "gui/units/UnitSystem.hpp"
#include "GroupDialog.hpp"

class DimensionsDialog: public GroupDialog
{
    Q_OBJECT

public:
    DimensionsDialog(QWidget* parent);

    Dimensions getData() const;
    void setData(const Dimensions& data);
    void setUnits(const UnitSystem& units);

signals:
    void modified();

private:
    DoubleEditor* edit0 = new DoubleEditor("Brace height");
    DoubleEditor* edit1 = new DoubleEditor("Draw length");
    DoubleEditor* edit2 = new DoubleEditor("Length");
    DoubleEditor* edit3 = new DoubleEditor("Setback");
    DoubleEditor* edit4 = new DoubleEditor("Angle");
};
