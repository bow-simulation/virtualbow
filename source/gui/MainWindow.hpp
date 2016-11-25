#pragma once
#include "../model/InputData.hpp"
#include <QtWidgets>

class BowEditor;

class MainWindow: public QMainWindow
{
public:
    MainWindow();

private:
    InputData input;
    BowEditor* editor;
    QString current_file;

    void closeEvent(QCloseEvent *event) override;

    void newFile();
    void open();
    bool save();
    bool saveAs();

    // Todo: settings, notes and about dialogs as lambdas in constructor?
    void settings();
    void comments();
    void runSimulation(bool dynamics);
    void about();

    void setCurrentFile(const QString &file_name);
    bool optionalSave();
    bool loadFile(const QString &file_name);
    bool saveFile(const QString &file_name);
};
