#include "EditableTabBar.hpp"

EditableTabBar::EditableTabBar()
{
    auto button = new QToolButton();
    int height = this->tabBar()->size().height();
    button->setIcon(QIcon(":/icons/plus"));
    button->setIconSize({height, height});
    button->setToolTip("Add Layer");

    this->setTabsClosable(true);
    this->setMovable(true);
    this->setCornerWidget(button, Qt::TopLeftCorner);

    QObject::connect(button, &QPushButton::clicked, this, &EditableTabBar::addTabRequested);
    QObject::connect(this, &QTabWidget::tabBarDoubleClicked, [&](int index)
    {
        QString old_text = this->tabText(index);
        this->setTabText(index, "...");

        RenameDialog dialog(this);
        dialog.setText(old_text);

        if(dialog.exec() == QDialog::Accepted)
        {
            QString new_text = dialog.getText();
            this->setTabText(index, new_text);
        }
        else
        {
            this->setTabText(index, old_text);
        }
    });
}

RenameDialog::RenameDialog(QWidget* parent)
    : QDialog(parent),
      editor(new QLineEdit())
{
    auto buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    QObject::connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    QObject::connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    auto vbox = new QVBoxLayout();
    vbox->addWidget(editor);
    vbox->addWidget(buttons);

    this->setWindowTitle("Rename");
    this->setLayout(vbox);
}

QString RenameDialog::getText() const
{
    return editor->text();
}

void RenameDialog::setText(QString text)
{
    editor->setText(text.remove('&'));    // Remove auto-mnemonics on KDE (https://stackoverflow.com/questions/32688153/how-to-disable-automatic-mnemonics-in-a-qt-application-on-kde)
    editor->selectAll();
}
