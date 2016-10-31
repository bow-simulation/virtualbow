#pragma once
#include "NumberGroup.hpp"
#include "../model/InputData.hpp"
#include <QtWidgets>

class SettingsDialog: public QDialog
{
public:
    SettingsDialog(QWidget* parent, InputData& data)
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
        group_dynamics->addRow("Step factor:", data.settings_step_factor);
        vbox->addWidget(group_dynamics);

        // OK Button
        auto hbox = new QHBoxLayout();
        auto btbox = new QDialogButtonBox(QDialogButtonBox::Ok);
        QObject::connect(btbox, &QDialogButtonBox::accepted, this, &QDialog::accept);
        vbox->addLayout(hbox);
        hbox->addWidget(btbox);

        this->setWindowTitle("Settings");
        this->layout()->setSizeConstraint(QLayout::SetFixedSize);
    }
};
