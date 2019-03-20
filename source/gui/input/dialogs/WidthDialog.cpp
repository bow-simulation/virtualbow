#include "WidthDialog.hpp"
#include "../editors/TreeEditor.hpp"

WidthDialog::WidthDialog(QWidget* parent)
    : PersistentDialog(parent, "WidthDialog", {800, 400}),    // Magic numbers
      edit(new SeriesEditor("Position", "Width [m]", 25)),
      view(new SplineView("Position", "Width [m]"))
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

    this->setWindowTitle("Width");
    this->setLayout(vbox);

    // Event handling

    QObject::connect(edit, &SeriesEditor::modified, [&]{
        view->setData(edit->getData());
        emit modified();
    });
}

Series WidthDialog::getData() const
{
    return edit->getData();
}

void WidthDialog::setData(const Series& width)
{
    QObject* p = parent();
    while (p) {
        if (p->objectName() == "tree_editor") {
            const std::vector<double>& args = static_cast<TreeEditor*>(p)->getData().profile.args();
            qreal limb_length = *std::max_element(args.begin(), args.end());
            edit->setHorizontalHeaderLabels({"Position [m]", "Width [m]"});
            edit->limb_length = limb_length;
            view->xAxis->setLabel("Position [m]");
            view->limb_length = limb_length;
            break;
        }
        p = p->parent();
    }
    edit->setData(width);
    view->setData(width);
}
