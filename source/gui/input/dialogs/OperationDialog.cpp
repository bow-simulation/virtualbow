#include "OperationDialog.hpp"

OperationDialog::OperationDialog(QWidget* parent)
    : GroupDialog(parent, "Operation", false)
{
    this->addWidget(edit0);
    this->addWidget(edit1);
    this->addWidget(edit2);

    QObject::connect(edit0, &DoubleEditor::modified, this, &OperationDialog::modified);
    QObject::connect(edit1, &DoubleEditor::modified, this, &OperationDialog::modified);
    QObject::connect(edit2, &DoubleEditor::modified, this, &OperationDialog::modified);
}

Operation OperationDialog::getData() const
{
    Operation data;
    data.brace_height = edit0->getData();
    data.draw_length = edit1->getData();
    data.arrow_mass = edit2->getData();

    return data;
}

void OperationDialog::setData(const Operation& data)
{
    edit0->setData(data.brace_height);
    edit1->setData(data.draw_length);
    edit2->setData(data.arrow_mass);
}
