#pragma once
#include "MainWindow.hpp"
#include <tclap/CmdLine.h>

/*
class Application
{
public:
    static int run_cli(int argc, char* argv[])
    {

    }

    static int run_gui(int argc, char* argv[])
    {
        QApplication app(argc, argv);
        app.setApplicationDisplayName("Bow Simulator");
        app.setApplicationVersion("0.2");
        app.setOrganizationDomain("https://bow-simulator.gitlab.io/");
        setlocale(LC_NUMERIC, "C");

        QStringList args = QApplication::arguments();
        MainWindow window(args.size() == 1 ? ":/bows/default.bow" : args.at(1));
        window.show();

        return app.exec();
    }
};
*/
