#include "DampingDialog.hpp"

DampingDialog::DampingDialog(QWidget* parent)
    : GroupDialog(parent, "Damping", false)
{
    this->addWidget(edit0);
    this->addWidget(edit1);

    QObject::connect(edit0, &DoubleEditor::modified, this, &DampingDialog::modified);
    QObject::connect(edit1, &DoubleEditor::modified, this, &DampingDialog::modified);
}

Damping DampingDialog::getData() const
{
    Damping data;
    data.damping_ratio_limbs = edit0->getData()/100.0;
    data.damping_ratio_string = edit1->getData()/100.0;

    return data;
}

void DampingDialog::setData(const Damping& data)
{
    edit0->setData(100.0*data.damping_ratio_limbs);
    edit1->setData(100.0*data.damping_ratio_string);
}
