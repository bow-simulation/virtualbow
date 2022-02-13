#include "LayersTreeItem.hpp"
#include "gui/viewmodel/DataViewModel.hpp"
#include "gui/widgets/TableEditor.hpp"
#include "gui/widgets/SplineView.hpp"
#include "gui/units/UnitSystem.hpp"

LayersTreeItem::LayersTreeItem(DataViewModel* model)
    : TreeItem("Layers", QIcon(":/icons/model-layers.svg"), TreeItemType::LAYERS),
      model(model)
{
    updateView();
}

void LayersTreeItem::updateModel() {
    std::vector<Layer> layers;
    for(int i = 0; i < this->childCount(); ++i) {
        auto item = dynamic_cast<LayerTreeItem*>(this->child(i));
        if(item != nullptr) {
            layers.push_back(item->getLayer());
        }
    }
    model->setLayers(layers);
}

void LayersTreeItem::updateView() {
    for(auto& layer: model->getLayers()) {
        auto item = new LayerTreeItem(model, layer);
        this->addChild(item);
    }
}

void LayersTreeItem::insertChild(int i, QTreeWidgetItem* item) {
    TreeItem::insertChild(i, item);
    updateModel();
}

void LayersTreeItem::removeChild(int i) {
    TreeItem::removeChild(i);
    updateModel();
}

void LayersTreeItem::swapChildren(int i, int j) {
    TreeItem::swapChildren(i, j);
    updateModel();
}

LayerTreeItem::LayerTreeItem(DataViewModel* model, const Layer& layer)
    : TreeItem(QString::fromStdString(layer.name), QIcon(":/icons/model-layer.svg"), TreeItemType::LAYER),
      model(model)
{
    table = new TableEditor("Position", "Height",
        TableSpinnerOptions(UnitSystem::ratio, DoubleRange::nonNegative(), 1e-4),
        TableSpinnerOptions(UnitSystem::length, DoubleRange::positive(), 1e-4)
    );

    combo = new QComboBox();
    updateCombo();

    table->setData(layer.height);
    combo->setCurrentIndex(layer.material);

    // Update model on changes
    QObject::connect(table, &TableEditor::modified, [&]{ updateModel(); });
    QObject::connect(combo, &QComboBox::currentTextChanged, [&]{ updateModel(); });

    // Update combobox on model changes
    QObject::connect(model, &DataViewModel::materialsModified, [&]{ updateCombo(); });

    auto hbox = new QHBoxLayout();
    hbox->addWidget(new QLabel("Material"));
    hbox->addWidget(combo);

    auto vbox = new QVBoxLayout();
    vbox->addLayout(hbox);
    vbox->addWidget(table);

    auto editor = new QWidget();
    editor->setLayout(vbox);
    setEditor(editor);

    auto plot = new SplineView("Position", "Height", UnitSystem::ratio, UnitSystem::length);
    setPlot(plot);

    this->setFlags(this->flags() | Qt::ItemIsEditable);
}

void LayerTreeItem::setData(int column, int role, const QVariant &value) {
    QTreeWidgetItem::setData(column, role, value);

    // Update view model on changes
    auto parent = dynamic_cast<LayersTreeItem*>(this->parent());
    if(parent != nullptr) {
        parent->updateModel();
    }
}

Layer LayerTreeItem::getLayer() const {
    return {
        .name = this->text(0).toStdString(),
        .material = combo->currentIndex(),
        .height = table->getData(),
        .rho = 0.0,
        .E = 0.0
    };
}

void LayerTreeItem::updateModel() {
    auto parent = dynamic_cast<LayersTreeItem*>(this->parent());
    if(parent != nullptr) {
        parent->updateModel();
    }
}

void LayerTreeItem::updateCombo() {
    QString selected = combo->currentText();

    combo->clear();
    for(auto& material: model->getMaterials()) {
        combo->addItem(QString::fromStdString(material.name));
    }

    combo->setCurrentText(selected);
}
