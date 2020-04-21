#include "MainWindow.hpp"
#include "config.hpp"
#include <iostream>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setOrganizationName(Config::ORGANIZATION_NAME);
    app.setOrganizationDomain(Config::ORGANIZATION_DOMAIN);
    app.setApplicationName(Config::APPLICATION_NAME_POST);
    app.setApplicationDisplayName(Config::APPLICATION_DISPLAY_NAME_POST);
    app.setApplicationVersion(Config::APPLICATION_VERSION);

    QLocale::setDefault(QLocale::C);

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("file", "Result file (.res)");
    parser.process(app);

    QStringList args = parser.positionalArguments();
    if(args.size() != 1) {
        std::cerr << "Wrong number of arguments." << std::endl;
        return 1;
    }

    MainWindow window(args[0]);
    window.show();
    return app.exec();
}
