#include "LayerDialog.hpp"

LayerDialog::LayerDialog(QWidget* parent, const UnitSystem& units)
    : PersistentDialog(parent, "LayerDialog", {800, 400}),    // Magic numbers
      tabs(new EditableTabBar()),
      units(units)
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

void LayerDialog::setData(const std::vector<Layer>& layers)
{
    while(tabs->count() < layers.size())
        createEmptyTab();

    while(tabs->count() > layers.size())
        tabs->removeTab(0);

    for(int i = 0; i < layers.size(); ++i)
        static_cast<LayerEditor*>(tabs->widget(i))->setData(layers[i]);
}

std::vector<Layer> LayerDialog::getData() const
{
    std::vector<Layer> layers;
    for(int i = 0; i < tabs->count(); ++i)
    {
        Layer layer = static_cast<LayerEditor*>(tabs->widget(i))->getData();
        layers.push_back(layer);
    }

    return layers;
}

LayerEditor* LayerDialog::createEmptyTab()
{
    auto editor = new LayerEditor(tabs, units);
    QObject::connect(editor, &LayerEditor::modified, this, &LayerDialog::modified);
    tabs->addTab(editor, "");

    return editor;
}

LayerEditor* LayerDialog::createDefaultTab()
{
    Layer layer;
    auto editor = createEmptyTab();
    editor->setData(layer);

    return editor;
}
