#pragma once
#include "bow/input/Operation.hpp"
#include "gui/input/editors/DoubleEditor.hpp"
#include "GroupDialog.hpp"

class OperationDialog: public GroupDialog
{
    Q_OBJECT

public:
    OperationDialog(QWidget* parent);

    Operation getData() const;
    void setData(const Operation& data);

signals:
    void modified();

private:
    DoubleEditor* edit0 = new DoubleEditor("Brace height [m]");
    DoubleEditor* edit1 = new DoubleEditor("Draw length [m]");
    DoubleEditor* edit2 = new DoubleEditor("Arrow mass [kg]");
};
