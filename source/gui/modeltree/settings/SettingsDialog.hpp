#pragma once
#include "solver/model/input/InputData.hpp"
#include "gui/widgets/IntegerEditor.hpp"
#include "gui/widgets/DoubleEditor.hpp"
#include "gui/units/UnitSystem.hpp"
#include "gui/modeltree/GroupDialog.hpp"

class SettingsDialog: public GroupDialog
{
    Q_OBJECT

public:
    SettingsDialog(QWidget* parent);

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
