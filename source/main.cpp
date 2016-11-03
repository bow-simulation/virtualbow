#include "gui/MainWindow.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    setlocale(LC_NUMERIC, "C");

    QGuiApplication::setApplicationDisplayName("Bow Simulator");
    QGuiApplication::setApplicationVersion("0.1.0");
    QGuiApplication::setOrganizationDomain("https://bow-simulator.sourceforge.net");

    MainWindow window;
    window.show();

    return app.exec();
}

/*
#include "model/BowModel.hpp"

int main()
{
    InputData2 input;
    input.load("../examples/layers.bow");

    OutputData output = BowModel::simulate(input, true, true);

    return 0;
}
*/
