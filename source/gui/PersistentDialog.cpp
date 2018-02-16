#include "PersistentDialog.hpp"
#include "Application.hpp"

PersistentDialog::PersistentDialog(QWidget* parent, const QString& name, const QSize& size)
    : BaseDialog(parent), name(name)
{
    // Load size
    resize(Application::settings.value(name + "/size", size).toSize());
}

PersistentDialog::~PersistentDialog()
{
    // Save size
    Application::settings.setValue(name + "/size", size());
}
