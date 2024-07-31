#pragma once
#include <QDialog>

// Base class for all input and output dialogs. Doesn't close on enter.
class DialogBase: public QDialog
{
public:
    DialogBase(QWidget* parent);
    void keyPressEvent(QKeyEvent *event) override;
};
