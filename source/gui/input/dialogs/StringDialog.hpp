#pragma once
#include "bow/input/String.hpp"
#include "gui/input/editors/IntegerEditor.hpp"
#include "gui/input/editors/DoubleEditor.hpp"
#include "GroupDialog.hpp"

class StringDialog: public GroupDialog
{
    Q_OBJECT

public:
    StringDialog(QWidget* parent);

    String getData() const;
    void setData(const String& data);

signals:
    void modified();

private:
    DoubleEditor* edit0 = new DoubleEditor("Strand stiffness [N/100%]");
    DoubleEditor* edit1 = new DoubleEditor("Strand density [kg/m]");
    IntegerEditor* edit2 = new IntegerEditor("Number of strands");
};
