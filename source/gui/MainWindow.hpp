#pragma once
#include "solver/model/input/InputData.hpp"
#include <QMainWindow>

class RecentFilesMenu;
class ViewModel;
class TreeView;
class LimbView;

class QLabel;

class MainWindow: public QMainWindow {
public:
    MainWindow();

    bool loadFromFile(const QString& path);
    bool saveToFile(const QString& path);

private:
    const char* DEFAULT_FILENAME = "Unnamed";
    const QSize DEFAULT_SIZE = { 1200, 700 };

    ViewModel* data_model;

    TreeView* tree_view;
    QLabel* property_view;
    QLabel* plot_view;
    LimbView* limb_view;

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

    InputData data;

    void setEditingEnabled(bool enabled);

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
