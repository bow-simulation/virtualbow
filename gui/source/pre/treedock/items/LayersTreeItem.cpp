#include "LayersTreeItem.hpp"
#include "pre/viewmodel/ViewModel.hpp"
#include "pre/widgets/TableEditor.hpp"
#include "pre/plotdock/plots/SplineView.hpp"
#include "pre/viewmodel/units/UnitSystem.hpp"

LayersTreeItem::LayersTreeItem(ViewModel* model)
    : TreeItem(model, "Layers", QIcon(":/icons/model-layers.svg"), TreeItemType::LAYERS)
{
    QObject::connect(model, &ViewModel::reloaded, [=] {
        initFromModel();
    });

    QObject::connect(model, &ViewModel::layerModified, [=](int i, void* source) {
        if(source != this) {
            auto item = dynamic_cast<LayerTreeItem*>(this->child(i));
            item->setLayer(model->getLayers()[i]);
        }
    });

    QObject::connect(model, &ViewModel::layerInserted, [=](int i, void* source) {
        if(source != this) {
            auto item = new LayerTreeItem(model);
            item->setLayer(model->getLayers()[i]);
            this->insertChild(i, item);
        }
    });

    QObject::connect(model, &ViewModel::layerRemoved, [=](int i, void* source) {
        if(source != this) {
            this->removeChild(i);
        }
    });

    QObject::connect(model, &ViewModel::layersSwapped, [=](int i, int j, void* source) {
        if(source != this) {
            this->swapChildren(i, j);
        }
    });

    initFromModel();
}

void LayersTreeItem::initFromModel() {
    this->removeChildren();
    for(auto& layer: model->getLayers()) {
        auto item = new LayerTreeItem(model);
        this->addChild(item);
        item->setLayer(layer);
    }
}

LayerTreeItem::LayerTreeItem(ViewModel* model)
    : TreeItem(model, "", QIcon(":/icons/model-layer.svg"), TreeItemType::LAYER)
{
    combo = new QComboBox();
    table = new TableEditor("Length", "Height", Quantities::ratio, Quantities::length, DoubleRange::nonNegative(1e-4), DoubleRange::nonNegative(1e-4));
    plot = new SplineView("Length", "Height", Quantities::ratio, Quantities::length);

    // Initialize combo box and update contents on changes to materials
    updateCombo();
    QObject::connect(model, &ViewModel::materialsModified, [=](void* source){
        if(source != parent()) {
            updateCombo();
        }
    });

    // Update data model on changes to the combo box selection
    QObject::connect(combo, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index){
        model->modifyLayer(row(), getLayer(), parent());
        updatePlot();    // TODO: Couple plot with model instead
    });

    // Update data model on changes to the table
    QObject::connect(table, &TableEditor::modified, [=]{
        model->modifyLayer(row(), getLayer(), parent());
        updatePlot();    // TODO: Couple plot with model instead
    });

    auto hbox = new QHBoxLayout();
    hbox->addWidget(new QLabel("Material"));
    hbox->addWidget(combo);

    auto vbox = new QVBoxLayout();
    vbox->addLayout(hbox);
    vbox->addWidget(table);

    auto editor = new QWidget();
    editor->setLayout(vbox);

    setEditor(editor);
    setPlot(new SplineView("Length", "Height", Quantities::ratio, Quantities::length));

    this->setFlags(this->flags() | Qt::ItemIsEditable);
}

Layer LayerTreeItem::getLayer() const {
    return {
        .name = this->text(0).toStdString(),
        .material = combo->currentIndex(),
        .height = table->getData()
    };
}

void LayerTreeItem::setLayer(const Layer& layer) {
    QSignalBlocker blocker1(combo);
    QSignalBlocker blocker2(table);

    this->setText(0, QString::fromStdString(layer.name));
    table->setData(layer.height);
    combo->setCurrentIndex(layer.material);

    updatePlot();    // TODO: Couple plot with model instead
}

void LayerTreeItem::setData(int column, int role, const QVariant &value) {
    QTreeWidgetItem::setData(column, role, value);
    if(role == Qt::EditRole) {
        model->modifyLayer(row(), getLayer(), parent());
    }
}

// Updates the contens of the combo box with the current list of materials
// Selects the same index as before the change. Modifications to the selection
// index are communicated separately by the model.
void LayerTreeItem::updateCombo() {
    QSignalBlocker blocker(combo);

    int index = combo->currentIndex();

    combo->clear();
    for(auto& material: model->getMaterials()) {
        combo->addItem(QString::fromStdString(material.name));
    }

    combo->setCurrentIndex(index);
}

void LayerTreeItem::updatePlot() {
    static_cast<SplineView*>(plot)->setData(model->getLayers()[this->row()].height);
}
