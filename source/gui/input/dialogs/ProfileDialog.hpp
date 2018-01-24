#pragma once
#include "gui/PersistentDialog.hpp"
#include "gui/input/views/ProfileView.hpp"
#include "gui/input/editors/LayerEditor.hpp"
#include "bow/input/InputData.hpp"

class ProfileDialog: public PersistentDialog
{
    Q_OBJECT

public:
    ProfileDialog(QWidget* parent)
        : PersistentDialog(parent, "ProfileDialog", {800, 400}),    // Magic numbers
          table(new SeriesEditor("Length [m]", "Curvature [1/m]", 25)),
          view(new ProfileView()),
          edit_x0(new DoubleEditor("X [m]")),
          edit_y0(new DoubleEditor("Y [m]")),
          edit_phi0(new DoubleEditor("Phi [rad]"))
    {
        auto buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        QObject::connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
        QObject::connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

        auto hbox1 = new QHBoxLayout();
        hbox1->setContentsMargins(10, 5, 10, 5);
        hbox1->addWidget(new QLabel("Offsets:"));
        hbox1->addSpacing(10);
        hbox1->addWidget(edit_x0);
        hbox1->addWidget(edit_y0);
        hbox1->addWidget(edit_phi0);
        hbox1->addStretch(1);

        auto group = new QGroupBox();
        group->setLayout(hbox1);

        auto vbox1 = new QVBoxLayout();
        vbox1->addWidget(group, 0);
        vbox1->addWidget(view, 1);

        auto hbox2 = new QHBoxLayout();
        hbox2->addWidget(table, 0);
        hbox2->addLayout(vbox1, 1);

        auto vbox2 = new QVBoxLayout();
        vbox2->addLayout(hbox2, 1);
        vbox2->addWidget(buttons, 0);

        this->setWindowTitle("Profile");
        this->setLayout(vbox2);

        // Event handling

        QObject::connect(table, &SeriesEditor::modified, this, &ProfileDialog::modified);
        QObject::connect(edit_x0, &DoubleEditor::modified, this, &ProfileDialog::modified);
        QObject::connect(edit_y0, &DoubleEditor::modified, this, &ProfileDialog::modified);
        QObject::connect(edit_phi0, &DoubleEditor::modified, this, &ProfileDialog::modified);

        QObject::connect(this, &ProfileDialog::modified, [&]{
            view->setData(this->getData());
        });
    }

    Profile2 getData() const
    {
        Profile2 profile;
        profile.segments = table->getData();
        profile.x0 = edit_x0->getData();
        profile.y0 = edit_y0->getData();
        profile.phi0 = edit_phi0->getData();

        return profile;
    }

    void setData(const Profile2& profile)
    {
        view->setData(profile);
        table->setData(profile.segments);
        edit_x0->setData(profile.x0);
        edit_y0->setData(profile.y0);
        edit_phi0->setData(profile.phi0);
    }

signals:
    void modified();

private:
    SeriesEditor* table;
    ProfileView* view;
    DoubleEditor* edit_x0;
    DoubleEditor* edit_y0;
    DoubleEditor* edit_phi0;
};
