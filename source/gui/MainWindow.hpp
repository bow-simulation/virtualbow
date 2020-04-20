#pragma once
#include "solver/model//input/InputData.hpp"
#include <QtWidgets>

class BowEditor;

class MainWindow: public QMainWindow
{
public:
    MainWindow();
    bool loadFile(const QString& path);
    bool saveFile(const QString& path);

private:
    const char* DEFAULT_FILENAME = "Unnamed";
    const int N_RECENT_FILES = 8;

    InputData data;
    BowEditor* editor;
    QString currentFile;

    QMenu* menu_recentfiles;
    QList<QAction*> recentFileActions;

    void closeEvent(QCloseEvent *event) override;

    void newFile();
    void open();
    void openRecent();
    bool save();
    bool saveAs();

    void runSimulation(const QString& flag);
    void about();

    void setCurrentFile(const QString& path);
    void setModified(bool modified);
    bool optionalSave();

    void clearRecentFilePaths();
    void addRecentFilePath(const QString& path);
    void updateRecentActionList();
};
