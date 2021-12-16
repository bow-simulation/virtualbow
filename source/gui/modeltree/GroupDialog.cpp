#include "GroupDialog.hpp"
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QAbstractButton>

GroupDialog::GroupDialog(QWidget* parent, const QString& title, bool enable_reset)
    : DialogBase(parent),
      vbox1(new QVBoxLayout()),
      vbox2(nullptr)
{
    auto flags = enable_reset ? QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Reset
                              : QDialogButtonBox::Ok | QDialogButtonBox::Cancel;

    auto buttons = new QDialogButtonBox(flags);
    QObject::connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    QObject::connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    QObject::connect(buttons, &QDialogButtonBox::clicked, [&, buttons](QAbstractButton* button){
        if(buttons->standardButton(button) == QDialogButtonBox::Reset)
            emit reset();
    });

    vbox1->addWidget(buttons);
    vbox1->setSizeConstraint(QLayout::SetFixedSize);    // Make dialog fixed-size
    this->setWindowTitle(title);
    this->setLayout(vbox1);
}

void GroupDialog::addGroup(const QString& name) {
    vbox2 = new QVBoxLayout();
    vbox2->setAlignment(Qt::AlignTop);

    auto group = new QGroupBox(name);
    group->setLayout(vbox2);

    vbox1->insertWidget(vbox1->count()-1, group, 1);
}

void GroupDialog::addWidget(QWidget* widget) {
    if(vbox2 == nullptr) {
        addGroup("");
    }

    vbox2->addWidget(widget);
}
