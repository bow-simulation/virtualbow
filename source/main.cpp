#include "gui/Application.hpp"

int main(int argc, char* argv[])
{
    return Application::run(argc, argv);
}

/*
#include <QtWidgets>
#include <random>

QColor getColor(double rho, double E)
{
    std::mt19937 rng(rho*E);    // random-number engine used (Mersenne-Twister in this case)
    std::uniform_int_distribution<int> uni(0, 255); // guaranteed unbiased

    return QColor(uni(rng), uni(rng), uni(rng));
}

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    QLabel label("ThisIsATestWithVeryLongText");
    label.show();

    double E = 1.2e12;
    double rho = 7500;

    QPalette palette = label.palette();
    palette.setColor(label.backgroundRole(), getColor(rho, E));
    label.setPalette(palette);

    return app.exec();
}
*/

/*
#include "gui/EditableTabBar.hpp"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    // QApplication::setStyle("windows");

    EditableTabBar tb;
    tb.addTab("Layer0");
    tb.addTab("Layer1");
    tb.addTab("Layer");
    tb.show();

    QObject::connect(&tb, &EditableTabBar::addTabRequested, []()
    {
        qInfo() << "Add Tab!";
    });

    QObject::connect(&tb, &EditableTabBar::tabRenamed, [](int index, const QString& name)
    {
        qInfo() << index << " renamed to " << name;
    });

    return app.exec();
}
*/
