#pragma once
#include "solver/model//input/Settings.hpp"
#include "gui/editors/IntegerEditor.hpp"
#include "gui/editors/DoubleEditor.hpp"
#include "GroupDialog.hpp"

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
    IntegerEditor* edit0 = new IntegerEditor("Limb elements");
    IntegerEditor* edit1 = new IntegerEditor("String elements");
    IntegerEditor* edit2 = new IntegerEditor("Draw steps");
    DoubleEditor* edit3 = new DoubleEditor("Time span factor");
    DoubleEditor* edit4 = new DoubleEditor("Time step factor");
    DoubleEditor* edit5 = new DoubleEditor("Sampling rate [Hz]");
    DoubleEditor* edit6 = new DoubleEditor("Arrow clamp force [N]");
};
