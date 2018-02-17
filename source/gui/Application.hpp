#pragma once
#include "config.hpp"
#include <QtGui>

class Application
{
public:
    static QSettings settings;
    static int run(int argc, char* argv[]);

private:
    static int runGUI(QApplication& app, QString path);
    static int runCLI(QString input_path, QString output_path, bool dynamic);
};
