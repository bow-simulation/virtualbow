#include "RecentFilesMenu.hpp"

RecentFilesMenu::RecentFilesMenu(QWidget* parent)
    : QMenu("Open &Recent", parent)
{
    const int N = 8;    // Number of retained paths

    // Create actions for opening recent files
    for(int i = 0; i < N; ++i) {
        auto action_open = new QAction(this);
        QObject::connect(action_open, &QAction::triggered, this, [=]{
            emit openRecent(action_open->data().toString());
        });
        recentFileActions.append(action_open);
        addAction(action_open);
    }

    // Create action for clearing the recent file list
    auto action_clear = new QAction("&Clear List", this);
    QObject::connect(action_clear, &QAction::triggered, this, [&]{
        clearPaths();
        updateActions();
    });
    this->addSeparator();
    this->addAction(action_clear);

    // Read initial paths from settings
    QSettings settings;
    int size = settings.beginReadArray("MainWindow/recentFiles");
    for(int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        recentFilePaths.append(settings.value("path").toString());
    }
    settings.endArray();
    updateActions();
}

RecentFilesMenu::~RecentFilesMenu()
{
    // Save recent paths to settings
    QSettings settings;
    settings.beginWriteArray("MainWindow/recentFiles");
    for(int i = 0; i < recentFilePaths.size(); ++i) {
        settings.setArrayIndex(i);
        settings.setValue("path", recentFilePaths[i]);
    }
    settings.endArray();
}

void RecentFilesMenu::addPath(const QString& path)
{
    recentFilePaths.removeAll(path);
    recentFilePaths.prepend(path);

    while(recentFilePaths.size() > recentFileActions.size())
        recentFilePaths.removeLast();

    updateActions();
}

void RecentFilesMenu::clearPaths()
{
    recentFilePaths.clear();
    updateActions();
}

void RecentFilesMenu::updateActions()
{
    for(int i = 0; i < recentFileActions.size(); ++i)
    {
        if(i > recentFilePaths.size() - 1)
        {
            recentFileActions[i]->setVisible(false);
        }
        else
        {
            QString path = recentFilePaths[i];
            QString name = QFileInfo(path).fileName();
            recentFileActions[i]->setText("&" + QString::number(i + 1) + " | " + name + " [ " + path + " ]");
            recentFileActions[i]->setData(path);
            recentFileActions[i]->setVisible(true);
        }
    }

    this->setEnabled(recentFilePaths.size() > 0);
}
