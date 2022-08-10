#include "HelpMenu.hpp"
#include "config.hpp"

#include <QCoreApplication>
#include <QMessageBox>
#include <QFileInfo>
#include <QDesktopServices>
#include <QUrl>

HelpMenu::HelpMenu(QWidget* parent)
    : QMenu("&Help", parent)
{
    auto action_help = new QAction("&User Manual", this);
    QObject::connect(action_help, &QAction::triggered, this, &HelpMenu::help);
    action_help->setShortcut(Qt::Key_F1);

    auto action_about = new QAction(QIcon(":/icons/dialog-information.svg"), "&About", this);
    QObject::connect(action_about, &QAction::triggered, this, &HelpMenu::about);
    action_about->setMenuRole(QAction::AboutRole);
    action_about->setIconVisibleInMenu(true);

    this->addAction(action_help);
    this->addAction(action_about);
}

void HelpMenu::help() {

#ifdef Q_OS_WIN
    // Location of the user manual on Windows
    QString path = QCoreApplication::applicationDirPath() + "/user-manual/index.html";
#endif

#ifdef Q_OS_LINUX
    // Location of the user manual on Linux
    QString path = "/usr/share/virtualbow/user-manual/index.html";
#endif

#ifdef Q_OS_MACOS
    // Location of the user manual on macOS
    QString path = QCoreApplication::applicationDirPath() + "/../Resources/user-manual/index.html";
#endif

    if(!QDesktopServices::openUrl(QUrl::fromLocalFile(path))) {
        QMessageBox::critical(this->parentWidget(), "Error", "Failed to open file " + path);
    }
}

void HelpMenu::about()
{
    QMessageBox::about(this->parentWidget(), "About", QString()
        + "<strong><font size=\"6\">VirtualBow</font></strong><br>"
        + "Version " + Config::APPLICATION_VERSION + "<br><br>"
        + Config::APPLICATION_DESCRIPTION + "<br>"
        + "<a href=\"" + Config::ORGANIZATION_DOMAIN + "\">" + Config::ORGANIZATION_DOMAIN + "</a><br><br>"
        + "<small>" + Config::APPLICATION_COPYRIGHT + "<br>"
        + "Distributed under the " + Config::APPLICATION_LICENSE + "</small>"
    );
}
