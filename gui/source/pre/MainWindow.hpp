#pragma once
#include <QMainWindow>

class RecentFilesMenu;
class MainVM;

class QLabel;

class MainWindow: public QMainWindow {
public:
    MainWindow();

    bool loadFromFile(const QString& path);
    bool saveToFile(const QString& path);

private:
    static QString DEFAULT_NAME;

    MainVM* viewModel;
    RecentFilesMenu* menuOpenRecent;

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
    QString displayPath();
};
