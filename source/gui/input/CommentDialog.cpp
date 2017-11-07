#include "CommentDialog.hpp"
#include "model/InputData.hpp"

CommentDialog::CommentDialog(QWidget* parent, InputData& data)
    : PersistentDialog(parent, "CommentDialog", {600, 300}),
      data(data)
{
    auto vbox = new QVBoxLayout();
    this->setLayout(vbox);
    this->setWindowTitle("Comments");
    this->setWindowFlags(this->windowFlags() | Qt::WindowMaximizeButtonHint);

    edit = new QTextEdit();
    edit->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));    // Set to system's default monospace font
    edit->setAcceptRichText(false);                                        // Limit to plain text
    edit->setText(QString::fromStdString(data.meta_comments));
    vbox->addWidget(edit);

    auto btbox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    vbox->addWidget(btbox);

    QObject::connect(btbox, &QDialogButtonBox::accepted, [this]
    {
        this->data.meta_comments = edit->toPlainText().toStdString();
        this->accept();
    });

    QObject::connect(btbox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}
