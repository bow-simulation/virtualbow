#include "ProfileDialog.hpp"

ProfileDialog::ProfileDialog(QWidget* parent)
    : PersistentDialog(parent, "ProfileDialog", { 800, 400 }),    // Magic numbers
      edit(new ProfileEditor()),
      view(new ProfileView(UnitSystem::length))
{
    auto buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    QObject::connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    QObject::connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    auto splitter = new QSplitter();
    splitter->addWidget(edit);
    splitter->addWidget(view);
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);
    splitter->setChildrenCollapsible(false);

    auto vbox = new QVBoxLayout();
    vbox->addWidget(splitter, 1);
    vbox->addWidget(buttons, 0);

    this->setWindowTitle("Profile");
    this->setLayout(vbox);

    // Event handling
    QObject::connect(edit, &ProfileEditor::selectionChanged, view, &ProfileView::setSelection);
    QObject::connect(edit, &ProfileEditor::modified, this, &ProfileDialog::modified);
    QObject::connect(this, &ProfileDialog::modified, [&]{
        view->setData(this->getData());
    });
}

std::vector<SegmentInput> ProfileDialog::getData() const {
    return edit->getData();
}

void ProfileDialog::setData(const std::vector<SegmentInput>& data) {
    view->setData(data);
    edit->setData(data);
}
