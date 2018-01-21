#pragma once
#include "bow/input2/Operation.hpp"
#include "../GroupDialog.hpp"
#include "../IntegerEditor.hpp"
#include "../DoubleEditor.hpp"

class OperationDialog: public GroupDialog
{
    Q_OBJECT

private:
    DoubleEditor* edit0 = new DoubleEditor("Brace height [m]");
    DoubleEditor* edit1 = new DoubleEditor("Draw length [m]");
    DoubleEditor* edit2 = new DoubleEditor("Arrow mass [kg]");

public:
    OperationDialog(QWidget* parent)
        : GroupDialog(parent, "Settings", false)
    {
        this->addWidget(edit0);
        this->addWidget(edit1);
        this->addWidget(edit2);

        QObject::connect(edit0, &DoubleEditor::modified, this, &OperationDialog::modified);
        QObject::connect(edit1, &DoubleEditor::modified, this, &OperationDialog::modified);
        QObject::connect(edit2, &DoubleEditor::modified, this, &OperationDialog::modified);
    }

    Operation2 getData() const
    {
        Operation2 data;
        data.brace_height = edit0->getData();
        data.draw_length = edit1->getData();
        data.mass_arrow = edit2->getData();

        return data;
    }

    void setData(const Operation2& data)
    {
        edit0->setData(data.brace_height);
        edit1->setData(data.draw_length);
        edit2->setData(data.mass_arrow);
    }

signals:
    void modified();
};
