/*
#include <iostream>
#include <array>
#include <cmath>

struct CurvePoint {
    double s;
    double x;
    double y;
    double phi;
};

class LineConstraint {
public:
    enum class Type { DELTA_S, DELTA_X, DELTA_Y };

    Type type;
    double value;

    LineConstraint(Type type, double value)
        : type(type), value(value) {
    }
};

class LineSegment {
public:
    LineSegment(CurvePoint startpoint, LineConstraint constraint) {
        double delta_s;

        switch(constraint.type) {
        case LineConstraint::Type::DELTA_S:
            delta_s = constraint.value;
            break;

        case LineConstraint::Type::DELTA_X:
            delta_s = constraint.value/cos(startpoint.phi);    // TODO
            break;

        case LineConstraint::Type::DELTA_Y:
            delta_s = constraint.value/sin(startpoint.phi);    // TODO
            break;
        }

        s0 = startpoint.s;
        s1 = s0 + delta_s;

        x0 = startpoint.x;
        y0 = startpoint.y;
        phi0 = startpoint.phi;
    }

    double t(double s) {
        return (s - s0)/(s1 - s0);
    }

    double s(double t) {
        return s0 + (s1 - s0)*t;
    }

    double x(double t) {
        return x0 + s(t)*cos(phi0);
    }

    double y(double t) {
        return x0 + s(t)*sin(phi0);
    }

    double phi(double t) {
        return phi0;
    }

private:
    double s0;
    double s1;

    double x0;
    double y0;
    double phi0;
};

class ArcConstraint {
public:
    enum class Type { DELTA_S, DELTA_X, DELTA_Y, DELTA_PHI, RADIUS };

    Type type;
    double value;

    ArcConstraint(Type type, double value)
        : type(type), value(value) {
    }
};

class ArcSegment {
public:
    ArcSegment(CurvePoint startpoint, ArcConstraint constraint) {
        double delta_s;

        // TODO

        s0 = startpoint.s;
        s1 = s0 + delta_s;

        x0 = startpoint.x;
        y0 = startpoint.y;
        phi0 = startpoint.phi;
    }

    double t(double s) {
        return (s - s0)/(s1 - s0);
    }

    double s(double t) {
        return s0 + (s1 - s0)*t;
    }

    double x(double t) {
        return x0 + s(t)*cos(phi0);
    }

    double y(double t) {
        return x0 + s(t)*sin(phi0);
    }

    double phi(double t) {
        return phi0;
    }

private:
    double s0;
    double s1;

    double x0;
    double y0;
    double phi0;
};



int main() {
    CurvePoint startpoint{.x = 0.5, .y = 0.0, .phi = 0.7};

    LineSegment line(startpoint, LineConstraint(LineConstraint::Type::DELTA_X, 5.0));

    std::cout << "Startpoint:\n";
    std::cout << line.x(0.0) << ", " << line.y(0.0) << "\n\n";

    std::cout << "Endpoint:\n";
    std::cout << line.x(1.0) << ", " << line.y(1.0) << "\n\n";
}
*/

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
