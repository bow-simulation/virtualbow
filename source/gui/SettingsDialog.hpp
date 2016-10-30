#pragma once
#include "Document.hpp"
#include <QtWidgets>

class SettingsDialog: public QDialog
{
public:
    SettingsDialog(QWidget* parent, Document& doc)
        : QDialog(parent)
    {
        auto vbox = new QVBoxLayout();
        this->setLayout(vbox);

        // General
        auto group_general = new DoubleGroup(doc, "General");
        group_general->addRow("Limb elements:", [](InputData& input)->unsigned&{ return input.settings.n_elements_limb; });
        group_general->addRow("String elements:", [](InputData& input)->unsigned&{ return input.settings.n_elements_string; });
        vbox->addWidget(group_general);

        // Statics
        auto group_statics = new DoubleGroup(doc, "Statics");
        group_statics->addRow("Draw steps:", [](InputData& input)->unsigned&{ return input.settings.n_draw_steps; });
        vbox->addWidget(group_statics);

        // Dynamics
        auto group_dynamics = new DoubleGroup(doc, "Dynamics");
        group_dynamics->addRow<DomainTag::NonNeg>("Step factor:", [](InputData& input)->double&{ return input.settings.step_factor; });
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

private:

};
