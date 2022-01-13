#pragma once
#include "solver/model/input/InputData.hpp"
#include "gui/widgets/DoubleEditor.hpp"
#include "gui/units/UnitSystem.hpp"
#include "gui/modeltree/GroupDialog.hpp"

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
    DoubleEditor* edit0;
    DoubleEditor* edit1;
    DoubleEditor* edit2;
    DoubleEditor* edit3;
    DoubleEditor* edit4;
};
