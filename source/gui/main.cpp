#include "Settings.hpp"
#include "MainWindow.hpp"
#include "config.hpp"
#include <iostream>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(Config::APPLICATION_NAME_GUI);
    app.setApplicationVersion(Config::APPLICATION_VERSION);
    app.setOrganizationDomain(Config::APPLICATION_WEBSITE);
    app.setAttribute(Qt::AA_DontShowIconsInMenus, true);

    QLocale::setDefault(QLocale::C);

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("file", "Model file (.bow)");
    parser.process(app);

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

     return app.exec();
}
