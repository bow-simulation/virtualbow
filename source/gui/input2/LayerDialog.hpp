#pragma once
#include "gui/PersistentDialog.hpp"
#include "gui/EditableTabBar.hpp"
#include "bow/input2/InputData.hpp"
#include "gui/input2/LayerEditor.hpp"

class LayerDialog: public PersistentDialog
{
    Q_OBJECT

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

        QObject::connect(tabs, &EditableTabBar::tabRenamed, this, &LayerDialog::modified);
        QObject::connect(tabs->tabBar(), &QTabBar::tabMoved, this, &LayerDialog::modified);

        QObject::connect(tabs, &EditableTabBar::addTabRequested, this, [&]{
            createDefaultTab();
            emit modified();
        });

        QObject::connect(tabs, &EditableTabBar::tabCloseRequested, [&](int index){
            tabs->removeTab(index);
            if(tabs->count() == 0)
                createDefaultTab();

            emit modified();
        });
    }

    void setData(const std::vector<Layer2>& layers)
    {
        while(tabs->count() < layers.size())
            createEmptyTab();

        while(tabs->count() > layers.size())
            tabs->removeTab(0);

        for(int i = 0; i < layers.size(); ++i)
        {
            tabs->setTabText(i, QString::fromStdString(layers[i].name));
            static_cast<LayerEditor*>(tabs->widget(i))->setData(layers[i]);
        }
    }

    std::vector<Layer2> getData() const
    {
        std::vector<Layer2> layers;
        for(int i = 0; i < tabs->count(); ++i)
        {
            Layer2 layer = static_cast<LayerEditor*>(tabs->widget(i))->getData();
            layer.name = tabs->tabText(i).toStdString();
            layers.push_back(layer);
        }

        return layers;
    }

signals:
    void modified();

private:
    EditableTabBar* tabs;

    void createEmptyTab()
    {
        auto editor = new LayerEditor();
        QObject::connect(editor, &LayerEditor::modified, this, &LayerDialog::modified);

        tabs->addTab(editor, "");
    }

    void createDefaultTab()
    {
        auto editor = new LayerEditor();
        QObject::connect(editor, &LayerEditor::modified, this, &LayerDialog::modified);

        Layer2 layer;
        tabs->addTab(editor, QString::fromStdString(layer.name));
        editor->setData(layer);
    }
};
