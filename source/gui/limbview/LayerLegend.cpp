#include "LayerLegend.hpp"
#include "LayerColors.hpp"
#include <QLabel>
#include <QHBoxLayout>

LayerEntry::LayerEntry()
    : symbol(new QLabel()),
      label(new QLabel())
{
    symbol->setFixedWidth(18);
    symbol->setFixedHeight(18);
    symbol->setScaledContents(true);
    label->setStyleSheet("color: white; font: 14px;");

    auto hbox = new QHBoxLayout();
    hbox->setAlignment(Qt::AlignTop);
    hbox->setMargin(0);
    hbox->addWidget(symbol, 0);
    hbox->addSpacing(10);
    hbox->addWidget(label, 1);
    this->setLayout(hbox);
}

void LayerEntry::setData(const Layer& layer, const std::vector<Material>& materials) {
    symbol->setPixmap(getLayerPixmap(layer, materials, 24));
    label->setText(QString::fromStdString(layer.name));
}

LayerLegend::LayerLegend()
    : vbox(new QVBoxLayout())
{
    vbox->setSpacing(2);
    this->setLayout(vbox);
}

void LayerLegend::setData(const std::vector<Layer>& layers, const std::vector<Material>& materials) {
    while(vbox->count() < layers.size()) {
        vbox->addWidget(new LayerEntry());
    }

    while(vbox->count() > layers.size()) {
        QWidget* widget = vbox->takeAt(0)->widget();
        widget->setParent(nullptr);
        widget->deleteLater();
    }

    for(int i = 0; i < vbox->count(); ++i) {
        LayerEntry* layer = dynamic_cast<LayerEntry*>(vbox->itemAt(i)->widget());
        layer->setData(layers[i], materials);
    }

    vbox->update();
}
