#pragma once
#include "solver/API.hpp"
#include <QMainWindow>

class RecentFilesMenu;
class MainModel;

class QLabel;

class MainWindow: public QMainWindow {
public:
    MainWindow();

    bool loadFromFile(const QString& path);
    bool saveToFile(const QString& path);

private:
    static QString DEFAULT_NAME;

    MainModel* mainModel;
    RecentFilesMenu* menuOpenRecent;

    void closeEvent(QCloseEvent *event) override;

    void newFile();
    void open();
    void openRecent(const QString& path);
    bool save();
    bool saveAs();

    void runSimulation(Mode mode);

    bool optionalSaveModifications();
    void submitChanges();

    QString showOpenFileDialog();
    QString showSaveFileDialog();
    QString displayPath();
};
