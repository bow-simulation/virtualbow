#pragma once
#include <QtGui>

class Application
{
public:
    static const std::string version;
    static QSettings settings;

    static int run(int argc, char* argv[]);

private:
    static int runGUI(QApplication& app, QString path);
    static int runCLI(QString input_path, QString output_path, bool dynamic);
};
