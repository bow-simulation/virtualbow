#pragma once
#include <QMainWindow>

class RecentFilesMenu;
class ViewModel;

class QLabel;

class MainWindow: public QMainWindow {
public:
    MainWindow();

    bool loadFromFile(const QString& path);
    bool saveToFile(const QString& path);

private:
    ViewModel* view_model;
    RecentFilesMenu* menu_open_recent;

    void closeEvent(QCloseEvent *event) override;

    void newFile();
    void open();
    void openRecent(const QString& path);
    bool save();
    bool saveAs();

    void runSimulation(bool dynamic);

    bool optionalSaveModifications();
    QString showOpenFileDialog();
    QString showSaveFileDialog();
};
