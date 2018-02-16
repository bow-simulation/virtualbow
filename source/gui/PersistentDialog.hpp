#pragma once
#include "gui/BaseDialog.hpp"

class PersistentDialog: public BaseDialog
{
public:
    PersistentDialog(QWidget* parent, const QString& name, const QSize& size);
    virtual ~PersistentDialog();

private:
    QString name;
};
