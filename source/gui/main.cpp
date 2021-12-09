#include "solver/model/profile/ProfileCurve.hpp"
#include <iostream>

int main() {
    Point start = Point{.s = 0.0, .angle = 0.1, .position = {0.0, 0.0}};

    LineInput line{{LineConstraint::LENGTH, 1.0}};
    ArcInput arc{{ArcConstraint::LENGTH, 1.0}, {ArcConstraint::K_START, 0.5}};
    ClothoidInput spiral{{ClothoidConstraint::LENGTH, 1.5}, {ClothoidConstraint::K_START, 0.5}, {ClothoidConstraint::K_END, 1.5}};

    ProfileCurve curve(start);
    curve.add_segment(line);
    curve.add_segment(arc);
    curve.add_segment(spiral);

    for(double s = 0.0; s <= curve.length(); s += 0.01) {
        auto position = curve.position(s);
        std::cout << position[0] << ", " << position[1] << "\n";
    }

    return 0;
}


/*
#include "MainWindow.hpp"
#include "config.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    QApplication::setOrganizationName(Config::ORGANIZATION_NAME);
    QApplication::setOrganizationDomain(Config::ORGANIZATION_DOMAIN);
    QApplication::setApplicationName(Config::APPLICATION_NAME_GUI);
    QApplication::setApplicationDisplayName(Config::APPLICATION_DISPLAY_NAME_GUI);
    QApplication::setApplicationVersion(Config::APPLICATION_VERSION);
    QLocale::setDefault(QLocale::C);

    QApplication application(argc, argv);
    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("file", "Model file (.bow)");
    parser.process(application);

    QStringList args = parser.positionalArguments();
    if(args.size() > 1) {
        std::cerr << "Only one argument is accepted." << std::endl;
        return 1;
    }

    MainWindow window;
    window.show();
    if(args.size() == 1) {
        window.loadFromFile(args[0]);
    }

    return application.exec();
}
*/
