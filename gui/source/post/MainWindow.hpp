#pragma once
#include "solver/model/output/OutputData.hpp"
#include "solver/model/output2/OutputData2.hpp"
#include <QMainWindow>

class RecentFilesMenu;

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

    OutputData2 data;

    void closeEvent(QCloseEvent *event) override;

    void open();
    void saveAs();
};
