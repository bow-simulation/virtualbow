#include "MainWindow.hpp"
#include "config.hpp"
#include <iostream>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setOrganizationName(Config::ORGANIZATION_NAME);
    app.setOrganizationDomain(Config::ORGANIZATION_DOMAIN);
    app.setApplicationName(Config::APPLICATION_NAME_GUI);
    app.setApplicationDisplayName(Config::APPLICATION_DISPLAY_NAME_GUI);
    app.setApplicationVersion(Config::APPLICATION_VERSION);
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

/*
#include <iostream>
#include "solver/model/ProfileCurve.hpp"
#include "solver/numerics/Linspace.hpp"
#include <vector>
#include <cmath>

int main() {
    std::vector<double> c{ 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0 };
    Segment segment(c);

    std::cout << segment.length() << "\n";
    std::cout << segment.angle() << "\n";
    std::cout << segment.energy() << "\n";
}
*/
