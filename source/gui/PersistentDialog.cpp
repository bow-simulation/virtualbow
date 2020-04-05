#include "PersistentDialog.hpp"
#include "Settings.hpp"

PersistentDialog::PersistentDialog(QWidget* parent, const QString& name, const QSize& size)
    : BaseDialog(parent), name(name)
{
    // Load size
    resize(SETTINGS.value(name + "/size", size).toSize());
}

PersistentDialog::~PersistentDialog()
{
    // Save size
    SETTINGS.setValue(name + "/size", size());
}
