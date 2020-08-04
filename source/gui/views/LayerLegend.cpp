#include "LayerLegend.hpp"

LayerEntry::LayerEntry()
    : symbol(new QLabel()),
      label(new QLabel())
{
    symbol->setFixedWidth(18);    // Magic number
    symbol->setFixedHeight(18);    // Magic number
    symbol->setScaledContents(true);
    label->setStyleSheet("color: white; font: 18px; max-height: 18px; background-color: transparent;");

    auto hbox = new QHBoxLayout();
    hbox->setMargin(2);   // Magic number
    hbox->addWidget(symbol, 0);
    hbox->addSpacing(10); // Magic number
    hbox->addWidget(label, 1);
    this->setLayout(hbox);
}

void LayerEntry::setData(const Layer& layer)
{
    symbol->setPixmap(getLayerPixmap(layer));
    label->setText(QString::fromStdString(layer.name));
}

LayerLegend::LayerLegend()
    : vbox(new QVBoxLayout())
{
    this->setLayout(vbox);
}

void LayerLegend::setData(const std::vector<Layer>& layers)
{
    while(vbox->count() < layers.size())
        vbox->addWidget(new LayerEntry());

    while(vbox->count() > layers.size())
    {
        QWidget* widget = vbox->takeAt(0)->widget();
        widget->setParent(nullptr);
        widget->deleteLater();
    }

    for(int i = 0; i < vbox->count(); ++i)
    {
        LayerEntry* layer = dynamic_cast<LayerEntry*>(vbox->itemAt(i)->widget());
        layer->setData(layers[i]);
    }

    vbox->update();
}
