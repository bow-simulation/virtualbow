#pragma once
#include "solver/model/input/Settings.hpp"
#include "gui/editors/IntegerEditor.hpp"
#include "gui/editors/DoubleEditor.hpp"
#include "gui/units/UnitSystem.hpp"
#include "GroupDialog.hpp"

class SettingsDialog: public GroupDialog
{
    Q_OBJECT

public:
    SettingsDialog(QWidget* parent, const UnitSystem& units);

    Settings getData() const;
    void setData(const Settings& data);

signals:
    void modified();

private:
    IntegerEditor* edit0;
    IntegerEditor* edit1;
    IntegerEditor* edit2;
    DoubleEditor* edit3;
    DoubleEditor* edit4;
    DoubleEditor* edit5;
    DoubleEditor* edit6;
};
