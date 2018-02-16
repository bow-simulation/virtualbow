#include "MassesDialog.hpp"

MassesDialog::MassesDialog(QWidget* parent)
    : GroupDialog(parent, "Masses", false)
{
    this->addWidget(edit0);
    this->addWidget(edit1);
    this->addWidget(edit2);

    QObject::connect(edit0, &DoubleEditor::modified, this, &MassesDialog::modified);
    QObject::connect(edit1, &DoubleEditor::modified, this, &MassesDialog::modified);
    QObject::connect(edit2, &DoubleEditor::modified, this, &MassesDialog::modified);
}

Masses MassesDialog::getData() const
{
    Masses data;
    data.string_center = edit0->getData();
    data.string_tip = edit1->getData();
    data.limb_tip = edit2->getData();

    return data;
}

void MassesDialog::setData(const Masses& data)
{
    edit0->setData(data.string_center);
    edit1->setData(data.string_tip);
    edit2->setData(data.limb_tip);
}
