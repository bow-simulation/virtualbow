#pragma once
#include "bow/input/String.hpp"
#include "gui/input/editors/IntegerEditor.hpp"
#include "gui/input/editors/DoubleEditor.hpp"
#include "GroupDialog.hpp"

class StringDialog: public GroupDialog
{
    Q_OBJECT

private:
    DoubleEditor* edit0 = new DoubleEditor("Strand stiffness [N/100%]");
    DoubleEditor* edit1 = new DoubleEditor("Strand density [kg/m]");
    IntegerEditor* edit2 = new IntegerEditor("Number of strands");

public:
    StringDialog(QWidget* parent)
        : GroupDialog(parent, "String", false)
    {
        this->addWidget(edit0);
        this->addWidget(edit1);
        this->addWidget(edit2);

        QObject::connect(edit0, &DoubleEditor::modified, this, &StringDialog::modified);
        QObject::connect(edit1, &DoubleEditor::modified, this, &StringDialog::modified);
        QObject::connect(edit2, &IntegerEditor::modified, this, &StringDialog::modified);
    }

    String2 getData() const
    {
        String2 data;
        data.strand_stiffness = edit0->getData();
        data.strand_density = edit1->getData();
        data.n_strands = edit2->getData();

        return data;
    }

    void setData(const String2& data)
    {
        edit0->setData(data.strand_stiffness);
        edit1->setData(data.strand_density);
        edit2->setData(data.n_strands);
    }

signals:
    void modified();
};
