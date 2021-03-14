#pragma once
#include "solver/model/input/InputData.hpp"
#include "units/UnitSystem.hpp"
#include <QtWidgets>

class ModelEditor;
class RecentFilesMenu;

class MainWindow: public QMainWindow {
public:
    MainWindow();

    bool loadFromFile(const QString& path);
    bool saveToFile(const QString& path);

private:
    const char* DEFAULT_FILENAME = "Unnamed";
    const QSize DEFAULT_SIZE = { 1200, 700 };

    QAction* action_new;
    QAction* action_open;
    QAction* action_save;
    QAction* action_save_as;
    QAction* action_quit;
    QAction* action_run_statics;
    QAction* action_run_dynamics;
    RecentFilesMenu* menu_open_recent;

    ModelEditor* editor;
    InputData data;
    UnitSystem units;

    void closeEvent(QCloseEvent *event) override;

    void newFile();
    void open();
    void openRecent(const QString& path);
    bool save();
    bool saveAs();

    void runSimulation(const QString& flag);

    bool optionalSaveModifications();
    QString showOpenFileDialog();
    QString showSaveFileDialog();

    void setFilePath(const QString& path);
    void setModified(bool modified);
};
