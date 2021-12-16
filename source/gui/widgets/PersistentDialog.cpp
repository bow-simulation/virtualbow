#include "PersistentDialog.hpp"
#include <QSettings>

PersistentDialog::PersistentDialog(QWidget* parent, const QString& name, const QSize& size)
    : DialogBase(parent), name(name)
{
    // Load size
    QSettings settings;
    resize(settings.value(name + "/size", size).toSize());
}

PersistentDialog::~PersistentDialog()
{
    // Save size
    QSettings settings;
    settings.setValue(name + "/size", size());
}
