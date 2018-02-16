#include "SettingsDialog.hpp"

SettingsDialog::SettingsDialog(QWidget* parent)
    : GroupDialog(parent, "Settings", true)
{
    this->addGroup("General");
    this->addWidget(edit0);
    this->addWidget(edit1);

    this->addGroup("Statics");
    this->addWidget(edit2);

    this->addGroup("Dynamics");
    this->addWidget(edit3);
    this->addWidget(edit4);
    this->addWidget(edit5);

    QObject::connect(edit0, &IntegerEditor::modified, this, &SettingsDialog::modified);
    QObject::connect(edit1, &IntegerEditor::modified, this, &SettingsDialog::modified);
    QObject::connect(edit2, &IntegerEditor::modified, this, &SettingsDialog::modified);
    QObject::connect(edit3, &DoubleEditor::modified, this, &SettingsDialog::modified);
    QObject::connect(edit4, &DoubleEditor::modified, this, &SettingsDialog::modified);
    QObject::connect(edit5, &DoubleEditor::modified, this, &SettingsDialog::modified);

    QObject::connect(this, &GroupDialog::reset, [&]{
        setData(Settings());
        emit modified();
    });
}

Settings SettingsDialog::getData() const
{
    Settings data;
    data.n_elements_limb = edit0->getData();
    data.n_elements_string = edit1->getData();
    data.n_draw_steps = edit2->getData();
    data.time_span_factor = edit3->getData();
    data.time_step_factor = edit4->getData();
    data.sampling_rate = edit5->getData();

    return data;
}

void SettingsDialog::setData(const Settings& data)
{
    edit0->setData(data.n_elements_limb);
    edit1->setData(data.n_elements_string);
    edit2->setData(data.n_draw_steps);
    edit3->setData(data.time_span_factor);
    edit4->setData(data.time_step_factor);
    edit5->setData(data.sampling_rate);
}
