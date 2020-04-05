#pragma once
#include "gui/PersistentDialog.hpp"
#include "solver/model/output/OutputData.hpp"
#include "solver/model/input/InputData.hpp"
#include <QtWidgets>

class MainWindow: public QMainWindow
{
public:
    MainWindow(const QString& path);

private:
    OutputData output;
    void closeEvent(QCloseEvent *event) override;
};

class OutputWidget: public QWidget
{
public:
    OutputWidget(const OutputData& output);
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
