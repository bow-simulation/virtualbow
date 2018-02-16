#pragma once
#include "bow/input/Masses.hpp"
#include "gui/input/editors/DoubleEditor.hpp"
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
    DoubleEditor* edit0 = new DoubleEditor("String center [kg]");
    DoubleEditor* edit1 = new DoubleEditor("String tip [kg]");
    DoubleEditor* edit2 = new DoubleEditor("Limb tip [kg]");
};
