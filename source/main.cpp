#include "gui/input2/LayerDialog.hpp"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    std::vector<Layer2> layers;
    layers.push_back({"layer 0", {{0.0, 1.0}, {0.01, 0.01}}, 5, 1000});
    layers.push_back({"layer 1", {{0.0, 1.0}, {0.01, 0.02}}, 7, 2000});
    layers.push_back({"layer 2", {{0.0, 1.0}, {0.01, 0.03}}, 9, 3000});

    LayerDialog dialog(nullptr);
    dialog.setData(layers);

    QObject::connect(&dialog, &LayerDialog::modified, []()
    {
        qInfo() << "Layers modified!";
    });

    dialog.exec();

    return app.exec();
}

/*
#include "gui/Application.hpp"

int main(int argc, char* argv[])
{
    return Application::run(argc, argv);
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
    tb.addTab("Layer2");
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
