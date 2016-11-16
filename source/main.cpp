#include "gui/MainWindow.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    setlocale(LC_NUMERIC, "C");

    QGuiApplication::setApplicationDisplayName("Bow Simulator");
    QGuiApplication::setApplicationVersion("0.1.0");
    QGuiApplication::setOrganizationDomain("https://bow-simulator.sourceforge.net");
    //QGuiApplication::setAttribute(Qt::AA_DontShowIconsInMenus, true);

    MainWindow window;
    window.show();

    return app.exec();
}
