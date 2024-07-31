#pragma once
#include <QMenu>

class HelpMenu: public QMenu
{
public:
    HelpMenu(QWidget* parent);

private:
    void help();
    void about();
};
