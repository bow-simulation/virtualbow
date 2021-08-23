#pragma once
#include "solver/model/input/String.hpp"
#include "gui/editors/IntegerEditor.hpp"
#include "gui/editors/DoubleEditor.hpp"
#include "gui/units/UnitSystem.hpp"
#include "GroupDialog.hpp"

class StringDialog: public GroupDialog {
    Q_OBJECT

public:
    StringDialog(QWidget* parent, const UnitSystem& units);

    String getData() const;
    void setData(const String& data);

signals:
    void modified();

private:
    DoubleEditor* edit0;
    DoubleEditor* edit1;
    IntegerEditor* edit2;
};
