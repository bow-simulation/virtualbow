#pragma once
#include "solver/model/input/Masses.hpp"
#include "gui/editors/DoubleEditor.hpp"
#include "gui/units/UnitSystem.hpp"
#include "GroupDialog.hpp"

class MassesDialog: public GroupDialog
{
    Q_OBJECT

public:
    MassesDialog(QWidget* parent, const UnitSystem& units);

    Masses getData() const;
    void setData(const Masses& data);

signals:
    void modified();

private:
    DoubleEditor* edit0;
    DoubleEditor* edit1;
    DoubleEditor* edit2;
    DoubleEditor* edit3;
};
