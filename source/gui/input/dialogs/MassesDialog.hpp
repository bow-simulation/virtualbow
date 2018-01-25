#pragma once
#include "bow/input/Masses.hpp"
#include "gui/input/editors/IntegerEditor.hpp"
#include "gui/input/editors/DoubleEditor.hpp"
#include "GroupDialog.hpp"

class MassesDialog: public GroupDialog
{
    Q_OBJECT

private:
    DoubleEditor* edit0 = new DoubleEditor("String center [kg]");
    DoubleEditor* edit1 = new DoubleEditor("String tip [kg]");
    DoubleEditor* edit2 = new DoubleEditor("Limb tip [kg]");

public:
    MassesDialog(QWidget* parent)
        : GroupDialog(parent, "Masses", false)
    {
        this->addWidget(edit0);
        this->addWidget(edit1);
        this->addWidget(edit2);

        QObject::connect(edit0, &DoubleEditor::modified, this, &MassesDialog::modified);
        QObject::connect(edit1, &DoubleEditor::modified, this, &MassesDialog::modified);
        QObject::connect(edit2, &DoubleEditor::modified, this, &MassesDialog::modified);
    }

    Masses getData() const
    {
        Masses data;
        data.string_center = edit0->getData();
        data.string_tip = edit1->getData();
        data.limb_tip = edit2->getData();

        return data;
    }

    void setData(const Masses& data)
    {
        edit0->setData(data.string_center);
        edit1->setData(data.string_tip);
        edit2->setData(data.limb_tip);
    }

signals:
    void modified();
};
