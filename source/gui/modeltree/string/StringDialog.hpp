#pragma once
#include "solver/model/input/InputData.hpp"
#include "gui/widgets/IntegerEditor.hpp"
#include "gui/widgets/DoubleEditor.hpp"
#include "gui/units/UnitSystem.hpp"
#include "gui/modeltree/GroupDialog.hpp"

class StringDialog: public GroupDialog {
    Q_OBJECT

public:
    StringDialog(QWidget* parent);

    String getData() const;
    void setData(const String& data);

signals:
    void modified();

private:
    DoubleEditor* edit0;
    DoubleEditor* edit1;
    IntegerEditor* edit2;
};
