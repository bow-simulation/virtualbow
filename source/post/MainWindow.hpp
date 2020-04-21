#pragma once
#include "gui/RecentFilesMenu.hpp"
#include <QtWidgets>

class MainWindow: public QMainWindow
{
public:
    MainWindow();
    void loadFile(const QString& path);
    void saveFile(const QString& path);

private:
    const char* DEFAULT_FILENAME = "Unnamed";
    RecentFilesMenu* menu_recent;

    void closeEvent(QCloseEvent *event) override;

    void open();
    void saveAs();
    void about();
};
