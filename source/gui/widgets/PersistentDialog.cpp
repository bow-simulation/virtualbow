#include "PersistentDialog.hpp"
#include "gui/utils/UserSettings.hpp"

PersistentDialog::PersistentDialog(QWidget* parent, const QString& name, const QSize& size)
    : DialogBase(parent), name(name)
{
    // Load size
    UserSettings settings;
    resize(settings.value(name + "/size", size).toSize());
}

PersistentDialog::~PersistentDialog() {
    // Save size
    UserSettings settings;
    settings.setValue(name + "/size", size());
}
