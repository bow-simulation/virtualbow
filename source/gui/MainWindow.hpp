#pragma once
#include "model/input/InputData.hpp"
#include <QtWidgets>

class BowEditor;

class MainWindow: public QMainWindow
{
public:
    MainWindow();
    ~MainWindow();

    bool loadFile(const QString& path);
    bool saveFile(const QString& path);

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
    void runSimulation(bool dynamic);
    void about();

    void setCurrentFile(const QString& path);
    bool optionalSave();
};
