#pragma once
#include "solver/model//input/Masses.hpp"
#include "gui/editors/DoubleEditor.hpp"
#include "GroupDialog.hpp"

class MassesDialog: public GroupDialog
{
    Q_OBJECT

public:
    MassesDialog(QWidget* parent);

    Masses getData() const;
    void setData(const Masses& data);

signals:
    void modified();

private:
    DoubleEditor* edit0 = new DoubleEditor("Arrow [kg]");
    DoubleEditor* edit1 = new DoubleEditor("String center [kg]");
    DoubleEditor* edit2 = new DoubleEditor("String tip [kg]");
    DoubleEditor* edit3 = new DoubleEditor("Limb tip [kg]");
};
