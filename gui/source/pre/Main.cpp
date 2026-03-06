#include "MainWindow.hpp"
#include "KeyEventFilter.hpp"
#include "config.hpp"
#include "solver/BowModel.hpp"
#include <iostream>

#include <QApplication>
#include <QCommandLineParser>

Q_DECLARE_METATYPE(LayerAlignment)
Q_DECLARE_METATYPE(ArrowMass)
Q_DECLARE_METATYPE(DrawLength)
Q_DECLARE_METATYPE(Handle)

int main(int argc, char* argv[]) {   
    QApplication::setOrganizationName(Config::ORGANIZATION_NAME);
    QApplication::setOrganizationDomain(Config::ORGANIZATION_DOMAIN);
    QApplication::setApplicationName(Config::APPLICATION_NAME_GUI);
    QApplication::setApplicationDisplayName(Config::APPLICATION_DISPLAY_NAME_GUI);
    QApplication::setApplicationVersion(Config::APPLICATION_VERSION);
    QLocale::setDefault(QLocale::C);

    qRegisterMetaType<LayerAlignment>("LayerAlignment");
    qRegisterMetaType<ArrowMass>("ArrowMass");
    qRegisterMetaType<DrawLength>("DrawLength");
    qRegisterMetaType<Handle>("Handle");

    QApplication application(argc, argv);
    application.installEventFilter(new KeyEventFilter());

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
