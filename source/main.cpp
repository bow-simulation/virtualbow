#include "gui/MainWindow.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    setlocale(LC_NUMERIC, "C");

    QGuiApplication::setApplicationDisplayName("Bow Simulator");
    QGuiApplication::setApplicationVersion("0.1.0");
    QGuiApplication::setOrganizationDomain("https://bow-simulator.sourceforge.net");

    QStringList args = QApplication::arguments();
    MainWindow window(args.size() == 1 ? ":/bows/default.bow" : args.at(1));
    window.show();

    return app.exec();
}

