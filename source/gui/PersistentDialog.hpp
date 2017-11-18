#pragma once
#include "gui/DialogBase.hpp"
#include "Application.hpp"

class PersistentDialog: public DialogBase
{
public:
    PersistentDialog(QWidget* parent, const QString& name, const QSize& size)
        : DialogBase(parent), name(name)
    {
        // Load size
        resize(Application::settings.getValue(name + "/size", size).toSize());
    }

    virtual ~PersistentDialog()
    {
        // Save size
        Application::settings.setValue(name + "/size", size());
    }

private:
    QString name;
};
