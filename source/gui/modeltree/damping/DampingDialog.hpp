#pragma once
#include "solver/model/input/Damping.hpp"
#include "gui/widgets/DoubleEditor.hpp"
#include "gui/units/UnitSystem.hpp"
#include "gui/modeltree/GroupDialog.hpp"

class DampingDialog: public GroupDialog
{
    Q_OBJECT

public:
    DampingDialog(QWidget* parent);

    Damping getData() const;
    void setData(const Damping& data);

signals:
    void modified();

private:
    DoubleEditor* edit0;
    DoubleEditor* edit1;
};
