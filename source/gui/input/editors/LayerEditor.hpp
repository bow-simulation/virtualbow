#pragma once
#include "gui/input/editors/SeriesEditor.hpp"
#include "gui/input/editors/DoubleEditor.hpp"
#include "gui/input/views/SplineView.hpp"
#include "bow/input/InputData.hpp"

class LayerEditor: public QWidget
{
    Q_OBJECT

public:
    LayerEditor()
        : table(new SeriesEditor("Position", "Height [m]", 25)),
          view(new SplineView("Position", "Height [m]")),
          edit_rho(new DoubleEditor("rho [kg/m³]")),
          edit_E(new DoubleEditor("E [Pa]"))
    {
        auto hbox1 = new QHBoxLayout();
        hbox1->setContentsMargins(10, 5, 10, 5);
        hbox1->addWidget(new QLabel("Offsets:"));
        hbox1->addWidget(edit_E);
        hbox1->addWidget(edit_rho);
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

        QObject::connect(edit_E, &DoubleEditor::modified, this, &LayerEditor::modified);
        QObject::connect(edit_rho, &DoubleEditor::modified, this, &LayerEditor::modified);

        QObject::connect(table, &SeriesEditor::modified, [&]{
            view->setData(table->getData());
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
        view->setData(layer.height);
        edit_E->setData(layer.E);
        edit_rho->setData(layer.rho);
    }

signals:
    void modified();

private:
    SeriesEditor* table;
    SplineView* view;
    DoubleEditor* edit_rho;
    DoubleEditor* edit_E;
};
