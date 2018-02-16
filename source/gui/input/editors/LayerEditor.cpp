#include "LayerEditor.hpp"

LayerEditor::LayerEditor(EditableTabBar* tabs)
    : tabs(tabs),
      table(new SeriesEditor("Position", "Height [m]", 25)),
      view(new SplineView("Position", "Height [m]")),
      edit_rho(new DoubleEditor("rho [kg/m³]")),
      edit_E(new DoubleEditor("E [Pa]"))
{
    auto hbox1 = new QHBoxLayout();
    hbox1->setContentsMargins(10, 5, 10, 5);
    hbox1->addWidget(new QLabel("Material:"));
    hbox1->addWidget(edit_rho);
    hbox1->addWidget(edit_E);
    hbox1->addStretch(1);

    auto group = new QGroupBox();
    group->setLayout(hbox1);

    auto vbox1 = new QVBoxLayout();
    vbox1->addWidget(group, 0);
    vbox1->addWidget(view, 1);

    auto hbox2 = new QHBoxLayout();
    hbox2->addWidget(table, 0);
    hbox2->addLayout(vbox1, 1);
    this->setLayout(hbox2);

    // Event handling

    QObject::connect(this, &LayerEditor::modified, this, &LayerEditor::updateTabIcon);

    QObject::connect(edit_E, &DoubleEditor::modified, this, &LayerEditor::modified);
    QObject::connect(edit_rho, &DoubleEditor::modified, this, &LayerEditor::modified);

    QObject::connect(table, &SeriesEditor::modified, [&]{
        view->setData(table->getData());
        emit modified();
    });
}

Layer LayerEditor::getData() const
{
    Layer layer;
    LayerEditor* wtf = const_cast<LayerEditor*>(this);
    layer.name = tabs->tabText(tabs->indexOf(wtf)).toStdString();
    layer.height = table->getData();
    layer.rho = edit_rho->getData();
    layer.E = edit_E->getData();

    return layer;
}

void LayerEditor::setData(const Layer& layer)
{
    tabs->setTabText(tabs->indexOf(this), QString::fromStdString(layer.name));
    tabs->setTabIcon(tabs->indexOf(this), QIcon(getLayerPixmap(layer)));
    table->setData(layer.height);
    view->setData(layer.height);
    edit_E->setData(layer.E);
    edit_rho->setData(layer.rho);
}

void LayerEditor::updateTabIcon()
{
    tabs->setTabIcon(tabs->indexOf(this), QIcon(getLayerPixmap(getData())));
}
