#pragma once
#include "bow/input/Damping.hpp"
#include "gui/input/editors/DoubleEditor.hpp"
#include "GroupDialog.hpp"

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
    DoubleEditor* edit0 = new DoubleEditor("Limbs [%]");
    DoubleEditor* edit1 = new DoubleEditor("String [%]");
};
