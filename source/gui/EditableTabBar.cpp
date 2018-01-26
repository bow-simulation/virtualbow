#include "EditableTabBar.hpp"

EditableTabBar::EditableTabBar()
{
    auto button = new QToolButton();
    int height = this->tabBar()->size().height();
    button->setIcon(QIcon(":/icons/plus"));
    button->setIconSize({height, height});
    button->setToolTip("Add Layer");

    this->setCornerWidget(button, Qt::TopLeftCorner);
    this->setTabsClosable(true);
    this->setMovable(true);

    QObject::connect(button, &QPushButton::clicked, this, &EditableTabBar::addTabRequested);
    QObject::connect(this, &QTabWidget::tabBarDoubleClicked, [&](int index)
    {
        QString old_text = this->tabText(index);
        this->setTabText(index, "...");

        QString new_text = QInputDialog::getText(this, "Rename", "New name:", QLineEdit::Normal, old_text);
        if(new_text != nullptr)
        {
            this->setTabText(index, new_text);
            emit tabRenamed(index);
        }
        else
        {
            this->setTabText(index, old_text);
        }
    });
}

// Removes auto-mnemonics inserted by KDE, https://stackoverflow.com/q/32688153/4692009
QString EditableTabBar::tabText(int index) const
{
    return QTabWidget::tabText(index).remove('&');
}
