#include "SettingsDialog.hpp"
#include "NumberGroup.hpp"
#include "../../model/InputData.hpp"

SettingsDialog::SettingsDialog(QWidget* parent, InputData& data)
    : QDialog(parent)
{
    auto vbox = new QVBoxLayout();
    this->setLayout(vbox);

    // General
    auto group_general = new NumberGroup(data, "General");
    group_general->addRow("Limb elements:", data.settings_n_elements_limb);
    group_general->addRow("String elements:", data.settings_n_elements_string);
    vbox->addWidget(group_general);

    // Statics
    auto group_statics = new NumberGroup(data, "Statics");
    group_statics->addRow("Draw steps:", data.settings_n_draw_steps);
    vbox->addWidget(group_statics);

    // Dynamics
    auto group_dynamics = new NumberGroup(data, "Dynamics");
    group_dynamics->addRow("Time span factor:", data.settings_time_span_factor);
    group_dynamics->addRow("Time step factor:", data.settings_time_step_factor);
    group_dynamics->addRow("Sampling time [s]:", data.settings_sampling_time);
    vbox->addWidget(group_dynamics);

    // OK Button
    auto btbox = new QDialogButtonBox(QDialogButtonBox::Ok);
    QObject::connect(btbox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    vbox->addWidget(btbox);

    this->setWindowTitle("Settings");
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    this->layout()->setSizeConstraint(QLayout::SetFixedSize);
}

// Keep dialog from closing on enter
void SettingsDialog::keyPressEvent(QKeyEvent *evt)
{
    if(evt->key() == Qt::Key_Enter || evt->key() == Qt::Key_Return)
        return;

    QDialog::keyPressEvent(evt);
}
