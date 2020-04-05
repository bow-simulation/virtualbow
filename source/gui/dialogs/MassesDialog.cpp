#include "MassesDialog.hpp"

MassesDialog::MassesDialog(QWidget* parent)
    : GroupDialog(parent, "Masses", false)
{
    this->addWidget(edit0);
    this->addWidget(edit1);
    this->addWidget(edit2);
    this->addWidget(edit3);

    QObject::connect(edit0, &DoubleEditor::modified, this, &MassesDialog::modified);
    QObject::connect(edit1, &DoubleEditor::modified, this, &MassesDialog::modified);
    QObject::connect(edit2, &DoubleEditor::modified, this, &MassesDialog::modified);
    QObject::connect(edit3, &DoubleEditor::modified, this, &MassesDialog::modified);
}

Masses MassesDialog::getData() const
{
    Masses data;
    data.arrow = edit0->getData();
    data.string_center = edit1->getData();
    data.string_tip = edit2->getData();
    data.limb_tip = edit3->getData();

    return data;
}

void MassesDialog::setData(const Masses& data)
{
    edit0->setData(data.arrow);
    edit1->setData(data.string_center);
    edit2->setData(data.string_tip);
    edit3->setData(data.limb_tip);
}
