#include "EditableTabBar.hpp"

EditableTabBar::EditableTabBar()
{
    button = new QPushButton("+", this);
    layout();

    setTabsClosable(true);
    setMovable(true);

    QObject::connect(button, &QPushButton::clicked, this, &EditableTabBar::addTabRequested);
    QObject::connect(this, &QTabBar::tabBarDoubleClicked, [&](int index)
    {
        RenameDialog dialog(this);
        dialog.setText(tabText(index));

        if(dialog.exec() == QDialog::Accepted)
        {
            QString text = dialog.getText();
            setTabText(index, text);
            emit tabRenamed(index, text);
        }
    });
}

void EditableTabBar::layout()
{
    int x = 0;
    for(int i = 0; i < count(); ++i)
        x += tabRect(i).width();

    int h = QTabBar::sizeHint().height();

    button->setFixedSize(h, h);
    button->move(x, 0);
}

QSize EditableTabBar::sizeHint() const
{
    // Return the size of the TabBar with increased width for the plus button.
    QSize sizeHint = QTabBar::sizeHint();
    sizeHint += {button->width(), 0};

    return sizeHint;
}

void EditableTabBar::resizeEvent(QResizeEvent * event)
{
    QTabBar::resizeEvent(event);
    layout();
}

void EditableTabBar::tabLayoutChange()
{
    QTabBar::tabLayoutChange();
    layout();
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
