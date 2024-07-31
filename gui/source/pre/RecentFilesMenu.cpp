#include "RecentFilesMenu.hpp"
#include "pre/utils/UserSettings.hpp"

#include <QFileInfo>

RecentFilesMenu::RecentFilesMenu(QWidget* parent)
    : QMenu("Open &Recent", parent)
{
    const int N = 8;    // Number of retained paths

    // Create fixed number of actions for opening recent files
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
    UserSettings settings;
    int size = settings.beginReadArray("MainWindow/recentFiles");
    for(int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        recentFilePaths.append(settings.value("path").toString());
    }
    settings.endArray();
}

RecentFilesMenu::~RecentFilesMenu() {
    // Save recent paths to settings
    UserSettings settings;
    settings.beginWriteArray("MainWindow/recentFiles");
    for(int i = 0; i < recentFilePaths.size(); ++i) {
        settings.setArrayIndex(i);
        settings.setValue("path", recentFilePaths[i]);
    }
    settings.endArray();
}

void RecentFilesMenu::addPath(const QString& path) {
    // Prepend the path to the list and remove any other occurrences
    recentFilePaths.removeAll(path);
    recentFilePaths.prepend(path);

    // Remove oldest files until list of paths has the same length as the action list
    while(recentFilePaths.size() > recentFileActions.size()) {
        recentFilePaths.removeLast();
    }
}

void RecentFilesMenu::clearPaths() {
    recentFilePaths.clear();
}

void RecentFilesMenu::updateActions() {
    // Remove paths that no longer exist
    for(int i = 0; i < recentFilePaths.size(); ++i) {
        if(!QFile::exists(recentFilePaths[i])) {
            recentFilePaths.removeAt(i);
        }
    }

    // Update actions from path list
    for(int i = 0; i < recentFileActions.size(); ++i) {
        if(i > recentFilePaths.size() - 1) {
            recentFileActions[i]->setVisible(false);
        }
        else {
            QString path = recentFilePaths[i];
            QString name = QFileInfo(path).fileName();
            recentFileActions[i]->setText("&" + QString::number(i + 1) + " | " + name + " [ " + path + " ]");
            recentFileActions[i]->setData(path);
            recentFileActions[i]->setVisible(true);
        }
    }

    this->setEnabled(!recentFilePaths.isEmpty());
}
