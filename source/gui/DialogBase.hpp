#pragma once
#include <QtWidgets>

// Base class for all input and output dialogs. Doesn't close on enter.
class DialogBase: public QDialog
{
public:
    DialogBase(QWidget* parent);
    virtual void keyPressEvent(QKeyEvent *event) override;
};
