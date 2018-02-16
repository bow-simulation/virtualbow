#include "LayerDialog.hpp"

LayerDialog::LayerDialog(QWidget* parent)
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

void LayerDialog::setData(const Layers& layers)
{
    while(tabs->count() < layers.size())
        createEmptyTab();

    while(tabs->count() > layers.size())
        tabs->removeTab(0);

    for(int i = 0; i < layers.size(); ++i)
        static_cast<LayerEditor*>(tabs->widget(i))->setData(layers[i]);
}

Layers LayerDialog::getData() const
{
    Layers layers;
    for(int i = 0; i < tabs->count(); ++i)
    {
        Layer layer = static_cast<LayerEditor*>(tabs->widget(i))->getData();
        layers.push_back(layer);
    }

    return layers;
}

LayerEditor* LayerDialog::createEmptyTab()
{
    auto editor = new LayerEditor(tabs);
    QObject::connect(editor, &LayerEditor::modified, this, &LayerDialog::modified);
    tabs->addTab(editor, "");

    return editor;
}

LayerEditor* LayerDialog::createDefaultTab()
{
    Layer layer;
    createEmptyTab()->setData(layer);
}
