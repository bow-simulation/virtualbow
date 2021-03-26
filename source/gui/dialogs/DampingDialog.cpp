#include "DampingDialog.hpp"

DampingDialog::DampingDialog(QWidget* parent, const UnitSystem& units)
    : GroupDialog(parent, "Damping", false)
{
    edit0 = new DoubleEditor("Limbs", units.ratio);
    edit1 = new DoubleEditor("String", units.ratio);

    this->addWidget(edit0);
    this->addWidget(edit1);

    QObject::connect(edit0, &DoubleEditor::modified, this, &DampingDialog::modified);
    QObject::connect(edit1, &DoubleEditor::modified, this, &DampingDialog::modified);
}

Damping DampingDialog::getData() const
{
    Damping data;
    data.damping_ratio_limbs = edit0->getData();
    data.damping_ratio_string = edit1->getData();

    return data;
}

void DampingDialog::setData(const Damping& data)
{
    edit0->setData(100.0*data.damping_ratio_limbs);
    edit1->setData(100.0*data.damping_ratio_string);
}
