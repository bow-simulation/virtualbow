#include "EditableTabBar.hpp"
#include <QTabBar>
#include <QToolButton>
#include <QPushButton>
#include <QInputDialog>

EditableTabBar::EditableTabBar()
{
    int tab_height = this->tabBar()->size().height();
    int font_height = this->fontMetrics().height();

    auto button = new QToolButton();
    button->setIcon(QIcon(":/icons/list-add"));
    button->setIconSize({tab_height, tab_height});        // Setting button size instead somehow makes the layout ugly
    button->setToolTip("Add Layer");

    this->setIconSize({font_height, font_height});
    this->setCornerWidget(button, Qt::TopLeftCorner);
    this->setTabsClosable(true);
    this->setMovable(true);

    QObject::connect(button, &QPushButton::clicked, this, &EditableTabBar::addTabRequested);
    QObject::connect(this, &QTabWidget::tabBarDoubleClicked, [&](int index)
    {
        QString old_text = this->tabText(index);
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
