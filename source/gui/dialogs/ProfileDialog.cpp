#include "ProfileDialog.hpp"

ProfileDialog::ProfileDialog(QWidget* parent)
    : PersistentDialog(parent, "ProfileDialog", {800, 400}),    // Magic numbers
      edit(new SeriesEditor("Length [m]", "Curvature [1/m]", 25)),
      view(new ProfileView())
{
    auto buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    QObject::connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    QObject::connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    auto hbox = new QHBoxLayout();
    hbox->addWidget(edit, 0);
    hbox->addWidget(view, 1);

    auto vbox = new QVBoxLayout();
    vbox->addLayout(hbox, 1);
    vbox->addWidget(buttons, 0);

    this->setWindowTitle("Profile");
    this->setLayout(vbox);

    // Event handling
    QObject::connect(edit, &SeriesEditor::rowSelectionChanged, view, &ProfileView::setSelection);
    QObject::connect(edit, &SeriesEditor::modified, this, &ProfileDialog::modified);
    QObject::connect(this, &ProfileDialog::modified, [&]{
        view->setData(this->getData());
    });
}

MatrixXd ProfileDialog::getData() const
{
    return edit->getData();
}

void ProfileDialog::setData(const MatrixXd& data)
{
    view->setData(data);
    edit->setData(data);
}
