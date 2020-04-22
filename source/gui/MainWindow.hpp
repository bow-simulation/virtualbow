#pragma once
#include "solver/model//input/InputData.hpp"
#include <QtWidgets>

class BowEditor;
class RecentFilesMenu;

class MainWindow: public QMainWindow
{
public:
    MainWindow();
    bool loadFile(const QString& path);
    bool saveFile(const QString& path);

private:
    const char* DEFAULT_FILENAME = "Unnamed";

    InputData data;
    BowEditor* editor;
    RecentFilesMenu* menu_recent;

    void closeEvent(QCloseEvent *event) override;

    void newFile();
    void open();
    void openRecent(const QString& path);
    bool save();
    bool saveAs();

    void runSimulation(const QString& flag);
    void help();
    void about();

    void setCurrentFile(const QString& path);
    void setModified(bool modified);
    bool optionalSave();
};
