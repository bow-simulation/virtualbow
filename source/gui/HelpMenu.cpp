#include "HelpMenu.hpp"
#include "config.hpp"

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

// Todo: Hard-coded paths?
void HelpMenu::help()
{
    QList<QString> paths = {
        QCoreApplication::applicationDirPath() + "/manual.pdf",                // Windows
        QCoreApplication::applicationDirPath() + "/../Resources/manual.pdf",   // MacOS
        "/usr/share/virtualbow/manual.pdf"                                     // Linux
    };

    for(const QString& path: paths) {
        if(QFileInfo::exists(path)) {
            if(!QDesktopServices::openUrl(QUrl::fromLocalFile(path))) {
                QMessageBox::critical(this->parentWidget(), "Error", "Failed to open file " + path);
            }
            return;
        }
    }

    QMessageBox::critical(this->parentWidget(), "Error", "Failed to open the user manual, file not found.");
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
