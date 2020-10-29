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
    try {
        // length 0.8, angle 0.5 fails with linear initial value
        std::vector<SegmentInput> input {
            { .length = 0.8, .angle = 0.5, .delta_x = std::nullopt, .delta_y = std::nullopt }
        };

        ProfileCurve curve(input);
        for(double t: Linspace(0.0, curve.length(), 50)) {
            Point point = curve(t);
            std::cout << point.x << "\t" << point.y << "\n";
        }
    }
    catch(std::runtime_error& e) {
        std::cout << e.what();
    }
}
*/
