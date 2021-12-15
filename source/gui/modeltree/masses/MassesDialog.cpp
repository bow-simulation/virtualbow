#include "MassesDialog.hpp"

MassesDialog::MassesDialog(QWidget* parent)
    : GroupDialog(parent, "Masses", false)
{
    edit_arrow = new DoubleEditor("Arrow", UnitSystem::mass);
    edit_string_center = new DoubleEditor("String center", UnitSystem::mass);
    edit_string_tip = new DoubleEditor("String tip", UnitSystem::mass);
    edit_limb_tip = new DoubleEditor("Limb tip", UnitSystem::mass);

    this->addWidget(edit_arrow);
    this->addWidget(edit_string_center);
    this->addWidget(edit_string_tip);
    this->addWidget(edit_limb_tip);

    QObject::connect(edit_arrow, &DoubleEditor::modified, this, &MassesDialog::modified);
    QObject::connect(edit_string_center, &DoubleEditor::modified, this, &MassesDialog::modified);
    QObject::connect(edit_string_tip, &DoubleEditor::modified, this, &MassesDialog::modified);
    QObject::connect(edit_limb_tip, &DoubleEditor::modified, this, &MassesDialog::modified);
}

Masses MassesDialog::getData() const {
    Masses data;
    data.arrow = edit_arrow->getData();
    data.string_center = edit_string_center->getData();
    data.string_tip = edit_string_tip->getData();
    data.limb_tip = edit_limb_tip->getData();

    return data;
}

void MassesDialog::setData(const Masses& data) {
    edit_arrow->setData(data.arrow);
    edit_string_center->setData(data.string_center);
    edit_string_tip->setData(data.string_tip);
    edit_limb_tip->setData(data.limb_tip);
}
