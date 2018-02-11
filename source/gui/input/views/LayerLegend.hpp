#pragma once
#include "bow/input/Layers.hpp"
#include "LayerColors.hpp"
#include <QtWidgets>

class LayerEntry: public QWidget
{
public:
    LayerEntry()
        : symbol(new QLabel()),
          label(new QLabel())
    {
        int size = symbol->sizeHint().height();
        symbol->setFixedWidth(size);
        symbol->setFixedHeight(size);
        symbol->setScaledContents(true);

        label->setAutoFillBackground(false);
        label->setStyleSheet("background-color: rgba(0, 0, 0, 0)");

        auto hbox = new QHBoxLayout();
        hbox->setMargin(0);
        hbox->addWidget(symbol, 0);
        hbox->addWidget(label, 1);
        this->setLayout(hbox);
        this->setStyleSheet("background-color: rgba(0, 0, 0, 0)");
    }

    void setData(const Layer& layer)
    {
        symbol->setPixmap(getLayerPixmap(layer));
        label->setText(QString::fromStdString(layer.name));
    }

private:
    QLabel* symbol;
    QLabel* label;
};

class LayerLegend: public QWidget
{
public:
    LayerLegend()
        : vbox(new QVBoxLayout())
    {
        this->setLayout(vbox);
        this->setStyleSheet("background-color: rgba(0, 0, 0, 0)");
    }

    void setData(const Layers& layers)
    {
        while(vbox->count() < layers.size())
            vbox->addWidget(new LayerEntry());    // Add new w

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

private:
    QVBoxLayout* vbox;
};
