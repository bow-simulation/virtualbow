#pragma once
#include "solver/model/input/Masses.hpp"
#include "gui/widgets/DoubleEditor.hpp"
#include "gui/units/UnitSystem.hpp"
#include "gui/modeltree/GroupDialog.hpp"

class MassesDialog: public GroupDialog {
    Q_OBJECT

public:
    MassesDialog(QWidget* parent);

    Masses getData() const;
    void setData(const Masses& data);

signals:
    void modified();

private:
    DoubleEditor* edit_arrow;
    DoubleEditor* edit_string_center;
    DoubleEditor* edit_string_tip;
    DoubleEditor* edit_limb_tip;
};
