#pragma once
#include "solver/model/input/InputData.hpp"
#include <QMainWindow>

class RecentFilesMenu;
class MainViewModel;
class DataViewModel;
class TreeView;
class PropertyView;
class PlotView;
class LimbView;

class QLabel;

class MainWindow: public QMainWindow {
public:
    MainWindow();

    bool loadFromFile(const QString& path);
    bool saveToFile(const QString& path);

private:
    MainViewModel* view_model;
    RecentFilesMenu* menu_open_recent;

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
};
