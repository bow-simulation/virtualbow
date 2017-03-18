#include "CommentsDialog.hpp"
#include "model/InputData.hpp"

CommentsDialog::CommentsDialog(QWidget* parent, InputData& data)
    : QDialog(parent),
      data(data)
{
    auto vbox = new QVBoxLayout();
    this->setLayout(vbox);
    this->setWindowTitle("Comments");
    this->setWindowFlags(this->windowFlags() | Qt::WindowMaximizeButtonHint);
    this->resize(600, 300);     // Todo: Magic numbers

    edit = new QTextEdit();
    edit->setText(QString::fromStdString(data.meta_comments));
    vbox->addWidget(edit);

    auto btbox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    vbox->addWidget(btbox);

    QObject::connect(btbox, &QDialogButtonBox::accepted, [this]()
    {
        this->data.meta_comments = edit->toPlainText().toStdString();
        this->accept();
    });

    QObject::connect(btbox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}
