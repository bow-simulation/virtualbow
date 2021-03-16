#pragma once
#include "solver/model/input/Damping.hpp"
#include "gui/editors/DoubleEditor.hpp"
#include "gui/units/UnitSystem.hpp"
#include "GroupDialog.hpp"

class DampingDialog: public GroupDialog
{
    Q_OBJECT

public:
    DampingDialog(QWidget* parent);

    Damping getData() const;
    void setData(const Damping& data);
    void setUnits(const UnitSystem& units);

signals:
    void modified();

private:
    DoubleEditor* edit0 = new DoubleEditor("Limbs");
    DoubleEditor* edit1 = new DoubleEditor("String");
};
