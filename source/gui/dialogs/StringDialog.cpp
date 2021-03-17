#include "StringDialog.hpp"

StringDialog::StringDialog(QWidget* parent, const UnitSystem& units)
    : GroupDialog(parent, "String", false)
{
    edit0 = new DoubleEditor("Strand stiffness", units.linear_stiffness);
    edit1 = new DoubleEditor("Strand density", units.linear_density);
    edit2 = new IntegerEditor("Number of strands");

    this->addWidget(edit0);
    this->addWidget(edit1);
    this->addWidget(edit2);

    QObject::connect(edit0, &DoubleEditor::modified, this, &StringDialog::modified);
    QObject::connect(edit1, &DoubleEditor::modified, this, &StringDialog::modified);
    QObject::connect(edit2, &IntegerEditor::modified, this, &StringDialog::modified);
}

String StringDialog::getData() const
{
    String data;
    data.strand_stiffness = edit0->getData();
    data.strand_density = edit1->getData();
    data.n_strands = edit2->getData();

    return data;
}

void StringDialog::setData(const String& data)
{
    edit0->setData(data.strand_stiffness);
    edit1->setData(data.strand_density);
    edit2->setData(data.n_strands);
}
