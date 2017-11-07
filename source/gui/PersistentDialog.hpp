#pragma once
#include <QDialog>

#include <QtCore>

class PersistentDialog: public QDialog
{
public:
    PersistentDialog(QWidget* parent, const QString& name, const QSize& size)
        : QDialog(parent), name(name)
    {
        // Load size
        QSettings settings;
        resize(settings.value(name + "/size", size).toSize());
    }

    virtual ~PersistentDialog()
    {
        // Save size
        QSettings settings;
        settings.setValue(name + "/size", size());
    }

private:
    QString name;
};
