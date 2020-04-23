#pragma once
#include <QtWidgets>

class HelpMenu: public QMenu
{
public:
    HelpMenu(QWidget* parent);

private:
    void help();
    void about();
};

