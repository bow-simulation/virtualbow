#include "MainWindow.hpp"
#include "config.hpp"
#include <QApplication>
#include <QCommandLineParser>
#include <iostream>

int main(int argc, char* argv[]) {
    QApplication::setOrganizationName(Config::ORGANIZATION_NAME);
    QApplication::setOrganizationDomain(Config::ORGANIZATION_DOMAIN);
    QApplication::setApplicationName(Config::APPLICATION_NAME_POST);
    QApplication::setApplicationDisplayName(Config::APPLICATION_DISPLAY_NAME_POST);
    QApplication::setApplicationVersion(Config::APPLICATION_VERSION);
    QLocale::setDefault(QLocale::C);

    QApplication application(argc, argv);
    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("file", "Result file (.res)");
    parser.process(application);

    QStringList args = parser.positionalArguments();
    if(args.size() > 1) {
        std::cerr << "Only one argument is accepted." << std::endl;
        return 1;
    }

    MainWindow window;
    window.show();
    if(args.size() == 1) {
        window.loadFile(args[0]);
    }

    return application.exec();
}
