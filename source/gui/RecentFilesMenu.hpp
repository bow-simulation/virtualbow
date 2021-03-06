#pragma once
#include <QtWidgets>

class RecentFilesMenu: public QMenu {
    Q_OBJECT

public:
    RecentFilesMenu(QWidget* parent);
    ~RecentFilesMenu();

    void addPath(const QString& path);
    void clearPaths();
    void updateActions();

private:
    QList<QAction*> recentFileActions;
    QList<QString> recentFilePaths;

signals:
    void openRecent(const QString& path);

};
