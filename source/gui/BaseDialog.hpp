#pragma once
#include <QtWidgets>

// Base class for all input and output dialogs. Doesn't close on enter.
class BaseDialog: public QDialog
{
public:
    BaseDialog(QWidget* parent);
    virtual void keyPressEvent(QKeyEvent *event) override;
};
