#pragma once
#include "gui/RecentFilesMenu.hpp"
#include "solver/model/output/OutputData.hpp"
#include "solver/model/input/InputData.hpp"
#include <QtWidgets>

class OutputWidget;

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

class OutputWidget: public QWidget
{
public:
    OutputWidget(const OutputData& output);
    const OutputData& getData();

private:
    OutputData data;
};

class StaticOutputWidget: public QWidget
{
public:
    StaticOutputWidget(const OutputData& output);
};

class DynamicOutputWidget: public QWidget
{
public:
    DynamicOutputWidget(const OutputData& output);
};
