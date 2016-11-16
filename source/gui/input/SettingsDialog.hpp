#pragma once
#include <QtWidgets>

class InputData;

class SettingsDialog: public QDialog
{
public:
    SettingsDialog(QWidget* parent, InputData& data);
    virtual void keyPressEvent(QKeyEvent *evt) override;
};
