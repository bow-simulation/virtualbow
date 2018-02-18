#include "ProfileDialog.hpp"

ProfileDialog::ProfileDialog(QWidget* parent)
    : PersistentDialog(parent, "ProfileDialog", {800, 400}),    // Magic numbers
      table(new SeriesEditor("Length [m]", "Curvature [1/m]", 25)),
      view(new ProfileView()),
      edit_x_pos(new DoubleEditor("X [m]")),
      edit_y_pos(new DoubleEditor("Y [m]")),
      edit_angle(new DoubleEditor("Angle [rad]"))
{
    auto buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    QObject::connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    QObject::connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    auto hbox1 = new QHBoxLayout();
    hbox1->setContentsMargins(10, 5, 10, 5);
    hbox1->addWidget(new QLabel("Offsets:"));
    hbox1->addSpacing(10);
    hbox1->addWidget(edit_x_pos);
    hbox1->addWidget(edit_y_pos);
    hbox1->addWidget(edit_angle);
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
    QObject::connect(edit_x_pos, &DoubleEditor::modified, this, &ProfileDialog::modified);
    QObject::connect(edit_y_pos, &DoubleEditor::modified, this, &ProfileDialog::modified);
    QObject::connect(edit_angle, &DoubleEditor::modified, this, &ProfileDialog::modified);

    QObject::connect(this, &ProfileDialog::modified, [&]{
        view->setData(this->getData());
    });
}

Profile ProfileDialog::getData() const
{
    Profile profile;
    profile.segments = table->getData();
    profile.x_pos = edit_x_pos->getData();
    profile.y_pos = edit_y_pos->getData();
    profile.angle = edit_angle->getData();

    return profile;
}

void ProfileDialog::setData(const Profile& profile)
{
    view->setData(profile);
    table->setData(profile.segments);
    edit_x_pos->setData(profile.x_pos);
    edit_y_pos->setData(profile.y_pos);
    edit_angle->setData(profile.angle);
}
