#include "DimensionsDialog.hpp"

DimensionsDialog::DimensionsDialog(QWidget* parent)
    : GroupDialog(parent, "Dimensions", false)
{
    this->addGroup("Draw");
    this->addWidget(edit0);
    this->addWidget(edit1);

    this->addGroup("Handle");
    this->addWidget(edit2);
    this->addWidget(edit3);
    this->addWidget(edit4);

    QObject::connect(edit0, &DoubleEditor::modified, this, &DimensionsDialog::modified);
    QObject::connect(edit1, &DoubleEditor::modified, this, &DimensionsDialog::modified);
    QObject::connect(edit2, &DoubleEditor::modified, this, &DimensionsDialog::modified);
    QObject::connect(edit3, &DoubleEditor::modified, this, &DimensionsDialog::modified);
    QObject::connect(edit4, &DoubleEditor::modified, this, &DimensionsDialog::modified);
}

Dimensions DimensionsDialog::getData() const
{
    Dimensions data;
    data.brace_height = edit0->getData();
    data.draw_length = edit1->getData();
    data.handle_length = edit2->getData();
    data.handle_setback = edit3->getData();
    data.handle_angle = edit4->getData();

    return data;
}

void DimensionsDialog::setData(const Dimensions& data)
{
    edit0->setData(data.brace_height);
    edit1->setData(data.draw_length);
    edit2->setData(data.handle_length);
    edit3->setData(data.handle_setback);
    edit4->setData(data.handle_angle);
}

void DimensionsDialog::setUnits(const UnitSystem& units)
{
    edit0->setUnit(units.length);
    edit1->setUnit(units.length);
    edit2->setUnit(units.length);
    edit3->setUnit(units.length);
    edit4->setUnit(units.angle);
}
