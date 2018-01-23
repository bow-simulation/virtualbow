#pragma once
#include "gui/views/DoubleView.hpp"
#include "gui/views/IntegerView.hpp"
#include "gui/BaseDialog.hpp"

class GroupDialog: public BaseDialog
{
public:
    GroupDialog(QWidget* parent, const QString& title, bool enable_reset)
        : BaseDialog(parent),
          vbox1(new QVBoxLayout()),
          vbox2(nullptr)
    {
        auto flags = QDialogButtonBox::Ok | QDialogButtonBox::Cancel;
        if(enable_reset)
        {
            flags = flags | QDialogButtonBox::Reset;
        }

        auto buttons = new QDialogButtonBox(flags);
        QObject::connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
        QObject::connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

        vbox1->addWidget(buttons); // , 0, Qt::AlignBottom);
        vbox1->setSizeConstraint(QLayout::SetFixedSize);    // Make dialog fixed-size
        this->setWindowTitle(title);
        this->setLayout(vbox1);
    }

    void addGroup(const QString& name)
    {
        vbox2 = new QVBoxLayout();
        vbox2->setAlignment(Qt::AlignTop);

        auto group = new QGroupBox(name);
        group->setLayout(vbox2);

        vbox1->insertWidget(vbox1->count()-1, group, 1);
    }

    void addWidget(QWidget* widget)
    {
        if(vbox2 == nullptr)
            addGroup("");

        vbox2->addWidget(widget);
    }

private:
    QVBoxLayout* vbox1;
    QVBoxLayout* vbox2;
};
