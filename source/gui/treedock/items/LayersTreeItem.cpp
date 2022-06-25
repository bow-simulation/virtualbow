#include "LayersTreeItem.hpp"
#include "gui/viewmodel/ViewModel.hpp"
#include "gui/widgets/TableEditor.hpp"
#include "gui/plotdock/plots/SplineView.hpp"
#include "gui/viewmodel/units/UnitSystem.hpp"

LayersTreeItem::LayersTreeItem(ViewModel* model)
    : TreeItem(model, "Layers", QIcon(":/icons/model-layers.svg"), TreeItemType::LAYERS)
{
    updateView(nullptr);
    QObject::connect(model, &ViewModel::layersModified, [=](void* source){
        updateView(source);
    });
}

void LayersTreeItem::updateModel(void* source) {
    std::vector<Layer> layers;
    for(int i = 0; i < this->childCount(); ++i) {
        auto item = dynamic_cast<LayerTreeItem*>(this->child(i));
        if(item != nullptr) {
            layers.push_back(item->getLayer());
        }
    }
    model->setLayers(layers, source);
}

void LayersTreeItem::updateView(void* source) {
    if(source != this) {
        // Remove children without notifying the model
        while(this->childCount() > 0) {
            QTreeWidgetItem::removeChild(this->child(0));
        }

        // Add new children
        for(auto& layer: model->getLayers()) {
            auto item = new LayerTreeItem(model, layer);
            this->addChild(item);
        }
    }
}

LayerTreeItem::LayerTreeItem(ViewModel* model, const Layer& layer)
    : TreeItem(model, QString::fromStdString(layer.name), QIcon(":/icons/model-layer.svg"), TreeItemType::LAYER)
{
    table = new TableEditor("Length", "Height", Quantities::ratio, Quantities::length, DoubleRange::nonNegative(1e-4), DoubleRange::positive(1e-4));

    combo = new QComboBox();
    updateCombo();

    setPlot(new SplineView("Length", "Height", Quantities::ratio, Quantities::length));
    updatePlot();

    table->setData(layer.height);
    combo->setCurrentIndex(layer.material);

    QObject::connect(combo, &QComboBox::currentTextChanged, [=]{ updateModel(parent()); });
    QObject::connect(table, &TableEditor::modified, [=]{
        updateModel(parent());
        updatePlot();
    });

    // Update combobox on model changes
    QObject::connect(model, &ViewModel::materialsModified, [&]{ updateCombo(); });

    auto hbox = new QHBoxLayout();
    hbox->addWidget(new QLabel("Material"));
    hbox->addWidget(combo);

    auto vbox = new QVBoxLayout();
    vbox->addLayout(hbox);
    vbox->addWidget(table);

    auto editor = new QWidget();
    editor->setLayout(vbox);
    setEditor(editor);

    this->setFlags(this->flags() | Qt::ItemIsEditable);
}

void LayerTreeItem::setData(int column, int role, const QVariant &value) {
    QTreeWidgetItem::setData(column, role, value);
    updateModel(parent());
}

Layer LayerTreeItem::getLayer() const {
    return {
        .name = this->text(0).toStdString(),
        .material = combo->currentIndex(),
        .height = table->getData()
    };
}

void LayerTreeItem::updateCombo() {
    QSignalBlocker blocker(combo);
    QString selected = combo->currentText();

    combo->clear();
    for(auto& material: model->getMaterials()) {
        combo->addItem(QString::fromStdString(material.name));
    }

    combo->setCurrentText(selected);
}

void LayerTreeItem::updatePlot() {
    static_cast<SplineView*>(plot)->setData(model->getLayers()[this->row()].height);
}
