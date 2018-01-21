#pragma once
#include "gui/input2/SeriesEditor.hpp"
#include "gui/input2/SplineView.hpp"
#include "gui/input2/DoubleEditor.hpp"
#include "bow/input2/InputData.hpp"

class LayerEditor: public QWidget
{
    Q_OBJECT

public:
    LayerEditor()
        : table(new SeriesEditor("Position", "Height [m]", 25)),
          plot(new SplineView("Position", "Height [m]")),
          edit_rho(new DoubleEditor("rho [kg/m³]")),
          edit_E(new DoubleEditor("E [Pa]"))
    {
        auto hbox1 = new QHBoxLayout();
        hbox1->setContentsMargins(10, 5, 10, 5);
        hbox1->addWidget(edit_E, 0);
        hbox1->addSpacing(10);
        hbox1->addWidget(edit_rho, 0);
        hbox1->addStretch(1);

        auto hbox2 = new QHBoxLayout();
        hbox2->setMargin(0);
        hbox2->addWidget(plot);

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

        // Event handling

        QObject::connect(edit_E, &DoubleEditor::modified, this, &LayerEditor::modified);
        QObject::connect(edit_rho, &DoubleEditor::modified, this, &LayerEditor::modified);

        QObject::connect(table, &SeriesEditor::modified, [&]{
            plot->setData(table->getData());
            emit modified();
        });
    }

    Layer2 getData() const
    {
        Layer2 layer;
        layer.height = table->getData();
        layer.rho = edit_rho->getData();
        layer.E = edit_E->getData();

        return layer;
    }

    void setData(const Layer2& layer)
    {
        table->setData(layer.height);
        plot->setData(layer.height);
        edit_E->setData(layer.E);
        edit_rho->setData(layer.rho);
    }

signals:
    void modified();

private:
    SeriesEditor* table;
    SplineView* plot;
    DoubleEditor* edit_rho;
    DoubleEditor* edit_E;
};
