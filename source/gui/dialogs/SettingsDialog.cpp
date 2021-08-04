#include "SettingsDialog.hpp"

SettingsDialog::SettingsDialog(QWidget* parent, const UnitSystem& units)
    : GroupDialog(parent, "Settings", true)
{
    edit0 = new IntegerEditor("Limb elements");
    edit1 = new IntegerEditor("String elements");
    edit2 = new IntegerEditor("Draw steps");
    edit3 = new DoubleEditor("Arrow clamp force", units.force);
    edit4 = new DoubleEditor("Time span factor", units.ratio);
    edit5 = new DoubleEditor("Time step factor", units.ratio);
    edit6 = new DoubleEditor("Sampling rate", units.frequency);

    this->addGroup("General");
    this->addWidget(edit0);
    this->addWidget(edit1);

    this->addGroup("Statics");
    this->addWidget(edit2);

    this->addGroup("Dynamics");
    this->addWidget(edit3);
    this->addWidget(edit4);
    this->addWidget(edit5);
    this->addWidget(edit6);

    QObject::connect(edit0, &IntegerEditor::modified, this, &SettingsDialog::modified);
    QObject::connect(edit1, &IntegerEditor::modified, this, &SettingsDialog::modified);
    QObject::connect(edit2, &IntegerEditor::modified, this, &SettingsDialog::modified);
    QObject::connect(edit3, &DoubleEditor::modified, this, &SettingsDialog::modified);
    QObject::connect(edit4, &DoubleEditor::modified, this, &SettingsDialog::modified);
    QObject::connect(edit5, &DoubleEditor::modified, this, &SettingsDialog::modified);
    QObject::connect(edit6, &DoubleEditor::modified, this, &SettingsDialog::modified);

    QObject::connect(this, &GroupDialog::reset, this, [&]{
        setData(Settings());
        emit modified();
    });
}

Settings SettingsDialog::getData() const {
    Settings data;
    data.n_limb_elements = edit0->getData();
    data.n_string_elements = edit1->getData();
    data.n_draw_steps = edit2->getData();
    data.arrow_clamp_force = edit3->getData();
    data.time_span_factor = edit4->getData();
    data.time_step_factor = edit5->getData();
    data.sampling_rate = edit6->getData();

    return data;
}

void SettingsDialog::setData(const Settings& data) {
    edit0->setData(data.n_limb_elements);
    edit1->setData(data.n_string_elements);
    edit2->setData(data.n_draw_steps);
    edit3->setData(data.arrow_clamp_force);
    edit4->setData(data.time_span_factor);
    edit5->setData(data.time_step_factor);
    edit6->setData(data.sampling_rate);
}
