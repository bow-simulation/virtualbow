#pragma once
#include "gui/RecentFilesMenu.hpp"
#include "gui/units/UnitDialog.hpp"
#include "solver/model/output/OutputData.hpp"
#include <QtWidgets>

class MainWindow: public QMainWindow {
public:
    MainWindow();
    void loadFile(const QString& path);
    void saveFile(const QString& path);

private:
    const char* DEFAULT_FILENAME = "Unnamed";
    const QSize INITIAL_SIZE = {1000, 700};

    QAction* action_open;
    QAction* action_save_as;
    QAction* action_quit;
    QAction* action_set_units;
    RecentFilesMenu* menu_open_recent;

    UnitSystem units;
    OutputData data;

    void closeEvent(QCloseEvent *event) override;

    void open();
    void saveAs();
};
