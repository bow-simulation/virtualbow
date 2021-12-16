#pragma once
#include "gui/widgets/DialogBase.hpp"

class PersistentDialog: public DialogBase
{
public:
    PersistentDialog(QWidget* parent, const QString& name, const QSize& size);
    ~PersistentDialog() override;

private:
    QString name;
};
