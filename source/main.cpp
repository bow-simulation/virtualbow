#include <string>
#include <vector>

#include "gui/TableWidget.hpp"
#include "gui/PersistentDialog.hpp"
#include "gui/EditableTabBar.hpp"
#include "gui/PlotWidget.hpp"

struct Layer
{
    std::string name = "";
    Series height = {{0.0, 1.0}, {0.01, 0.01}};
    double rho = 5000.0;
    double E = 1.0e12;
};

using Layers = std::vector<Layer>;

class DoubleEditor: public QWidget
{
public:
    DoubleEditor(const QString& text)
        : label(new QLabel(text)),
          edit(new QLineEdit())
    {
        edit->setValidator(new QDoubleValidator());

        auto hbox = new QHBoxLayout();
        hbox->addWidget(label);
        hbox->addWidget(edit);

        this->setLayout(hbox);
    }

    void setData(double data)
    {
        edit->setText(QLocale::c().toString(data, 'g'));
    }

    double getData() const
    {
        return QLocale::c().toDouble(edit->text());
    }

private:
    QLabel* label;
    QLineEdit* edit;
};

class LayerEditor: public QWidget
{
public:
    LayerEditor()
        : table(new TableWidget("Position", "Height [m]", 25)),
          plot(new PlotWidget()),
          edit_rho(new DoubleEditor("rho [kg/m³]")),
          edit_E(new DoubleEditor("E [Pa]"))
    {
        auto hbox1 = new QHBoxLayout();
        hbox1->setContentsMargins(10, 0, 10, 0);
        hbox1->addWidget(edit_E);
        hbox1->addWidget(edit_rho);
        hbox1->addStretch();

        auto hbox2 = new QHBoxLayout();
        hbox2->setMargin(0);
        hbox2->addWidget(plot);
        plot->xAxis->setLabel("Position");
        plot->yAxis->setLabel("Height [m]");

        auto gbox1 = new QGroupBox();
        gbox1->setLayout(hbox1);

        auto gbox2 = new QGroupBox();
        gbox2->setLayout(hbox2);

        auto vbox = new QVBoxLayout();
        vbox->addWidget(gbox1, 0);
        vbox->addWidget(gbox2, 1);

        auto hbox = new QHBoxLayout();
        hbox->addWidget(table, 0);
        hbox->addLayout(vbox, 1);

        this->setLayout(hbox);
    }

    void setData(const Layer& layer)
    {
        table->setData(layer.height);
        edit_E->setData(layer.E);
        edit_rho->setData(layer.rho);
    }

private:
    TableWidget* table;
    PlotWidget* plot;
    DoubleEditor* edit_rho;
    DoubleEditor* edit_E;
};

class LayerDialog: public PersistentDialog
{
public:
    LayerDialog(QWidget* parent)
        : PersistentDialog(parent, "LayerDialog", {800, 400}),    // Magic numbers
          tabs(new EditableTabBar())
    {
        auto buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        QObject::connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
        QObject::connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

        auto vbox = new QVBoxLayout();
        vbox->addWidget(tabs, 1);
        vbox->addWidget(buttons, 0);

        this->setWindowTitle("Layers");
        this->setLayout(vbox);

        // Event handling

        QObject::connect(tabs, &EditableTabBar::addTabRequested, this, &LayerDialog::createDefaultTab);
        QObject::connect(tabs, &EditableTabBar::tabCloseRequested, [&](int index)
        {
            tabs->removeTab(index);
            if(tabs->count() == 0)
            {
                createDefaultTab();
            }
        });
    }

    void setData(const Layers& layers)
    {
        while(tabs->count() < layers.size())
        {
            tabs->addTab(new LayerEditor(), "");
        }

        while(tabs->count() > layers.size())
        {
            tabs->removeTab(0);
        }

        for(int i = 0; i < layers.size(); ++i)
        {
            tabs->setTabText(i, QString::fromStdString(layers[i].name));
            static_cast<LayerEditor*>(tabs->widget(i))->setData(layers[i]);
        }
    }

private:
    EditableTabBar* tabs;

    void createDefaultTab()
    {
        auto editor = new LayerEditor();
        editor->setData(Layer());

        tabs->addTab(editor, "unnamed");
    }
};



int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    Layers layers;
    layers.push_back({"layer 0", {{0.0, 1.0}, {0.01, 0.01}}, 5, 1000});
    layers.push_back({"layer 1", {{0.0, 1.0}, {0.01, 0.02}}, 7, 2000});
    layers.push_back({"layer 2", {{0.0, 1.0}, {0.01, 0.03}}, 9, 3000});

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
