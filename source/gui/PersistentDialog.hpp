#pragma once
#include "Application.hpp"
#include <QDialog>

class PersistentDialog: public QDialog
{
public:
    PersistentDialog(QWidget* parent, const QString& name, const QSize& size)
        : QDialog(parent), name(name)
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
