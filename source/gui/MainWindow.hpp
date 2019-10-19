#pragma once
#include "bow/input/InputData.hpp"
#include <QtWidgets>

class BowEditor;

class MainWindow: public QMainWindow
{
public:
    MainWindow();
    ~MainWindow();

    bool loadFile(const QString& path);
    bool saveFile(const QString& path);

private:
    InputData data;
    BowEditor* editor;
    QString current_file;

    void closeEvent(QCloseEvent *event) override;

    void newFile();
    void open();
    void openRecent();
    bool save();
    bool saveAs();

    // Todo: settings, notes and about dialogs as lambdas in constructor?
    void settings();
    void comments();
    void runSimulation(bool dynamic);
    void about();

    void setCurrentFile(const QString& path);
    bool optionalSave();

    QList<QAction*> recentFileActionList;
    QStringList recentFilePaths;
    void updateRecentActionList();
    void readRecentFilePaths();
    void updateRecentFilePaths(const QString& path);
    void saveRecentFilePaths();
};
