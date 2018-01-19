#include <string>
#include <vector>

#include "gui/TableWidget.hpp"
#include "gui/PersistentDialog.hpp"
#include "gui/EditableTabBar.hpp"

struct Layer
{
    std::string name;
    Series height;
    double rho;
    double E;
};

using Layers = std::vector<Layer>;

class LayerDialog: public PersistentDialog
{
public:
    LayerDialog(QWidget* parent)
        : PersistentDialog(parent, "LayerDialog", {800, 400})    // Magic numbers
    {
        // Widgets

        tabs = new EditableTabBar();

        // Layout

        auto vbox = new QVBoxLayout();
        vbox->addWidget(tabs);

        this->setWindowTitle("Layers");
        this->setLayout(vbox);

        // Event handling
    }

    void setData(const Layers& layers)
    {
        for(auto& layer: layers)
            tabs->addTab(new QLabel("Content"), QString::fromStdString(layer.name));
    }

private:
    QTabWidget* tabs;
};



int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    Layers layers;
    layers.push_back({"layer 0", {{0.0, 1.0}, {0.01, 0.01}}, 1000, 5});
    layers.push_back({"layer 1", {{0.0, 1.0}, {0.01, 0.02}}, 2000, 7});
    layers.push_back({"layer 2", {{0.0, 1.0}, {0.01, 0.03}}, 3000, 9});

    LayerDialog dialog(nullptr);
    dialog.setData(layers);
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
