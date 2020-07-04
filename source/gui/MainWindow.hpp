#pragma once
#include "solver/model/input/InputData.hpp"
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
    const QSize INITIAL_SIZE = {1200, 700};

    QAction* action_new;
    QAction* action_open;
    QAction* action_save;
    QAction* action_save_as;
    QAction* action_quit;
    QAction* action_run_statics;
    QAction* action_run_dynamics;
    RecentFilesMenu* menu_recent;

    InputData data;
    BowEditor* editor;

    void closeEvent(QCloseEvent *event) override;

    void newFile();
    void open();
    void openRecent(const QString& path);
    bool save();
    bool saveAs();

    void runSimulation(const QString& flag);

    void setCurrentFile(const QString& path);
    void setModified(bool modified);
    void setEditingEnabled(bool enabled);
    bool optionalSave();
};
