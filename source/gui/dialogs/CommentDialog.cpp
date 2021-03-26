#include "CommentDialog.hpp"

CommentDialog::CommentDialog(QWidget* parent, const UnitSystem& units)
    : PersistentDialog(parent, "CommentDialog", {600, 300}),
      edit(new QTextEdit()),
      changed(false)
{
    edit->setAcceptRichText(false);    // Limit to plain text
    edit->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));    // Use system's default monospace font

    auto buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    auto vbox = new QVBoxLayout();
    vbox->addWidget(edit);
    vbox->addWidget(buttons);

    this->setLayout(vbox);
    this->setWindowTitle("Comments");

    QObject::connect(edit, &QTextEdit::textChanged, [&]{
        changed = true;
    });

    QObject::connect(buttons, &QDialogButtonBox::accepted, [&]{
        emit CommentDialog::accept();
        if(changed)
            emit modified();
    });

    QObject::connect(buttons, &QDialogButtonBox::rejected, this, &CommentDialog::reject);
}

std::string CommentDialog::getData() const
{
    return edit->toPlainText().toStdString();
}

void CommentDialog::setData(const std::string& text)
{
    edit->setText(QString::fromStdString(text));
}
