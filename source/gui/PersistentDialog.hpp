#pragma once
#include "gui/BaseDialog.hpp"

class PersistentDialog: public BaseDialog
{
public:
    PersistentDialog(QWidget* parent, const QString& name, const QSize& size);
    ~PersistentDialog() override;

private:
    QString name;
};
