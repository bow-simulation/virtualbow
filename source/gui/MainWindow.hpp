#pragma once
#include "solver/model/input/InputData.hpp"
#include <QMainWindow>

class RecentFilesMenu;
class MainViewModel;
class DataViewModel;
class TreeView;
class PropertyView;
class LimbView;

class QLabel;

class MainWindow: public QMainWindow {
public:
    MainWindow();

private:
    const char* DEFAULT_FILENAME = "Unnamed";
    const QSize DEFAULT_SIZE = { 1200, 700 };

    MainViewModel* view_model;

    QDockWidget* dock_tree_view;
    QDockWidget* dock_property_view;
    QDockWidget* dock_plot_view;

    QAction* action_new;
    QAction* action_open;
    QAction* action_save;
    QAction* action_save_as;
    QAction* action_quit;
    QAction* action_run_statics;
    QAction* action_run_dynamics;
    QAction* action_set_units;

    RecentFilesMenu* menu_open_recent;

    void closeEvent(QCloseEvent *event) override;

    bool loadFromFile(const QString& path);
    bool saveToFile(const QString& path);

    void newFile();
    void open();
    void openRecent(const QString& path);
    bool save();
    bool saveAs();

    void runSimulation(const QString& flag);

    bool optionalSaveModifications();
    QString showOpenFileDialog();
    QString showSaveFileDialog();
};
