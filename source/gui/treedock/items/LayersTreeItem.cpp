#include "LayersTreeItem.hpp"
#include "gui/viewmodel/ViewModel.hpp"
#include "gui/widgets/TableEditor.hpp"
#include "gui/plotdock/plots/SplineView.hpp"
#include "gui/viewmodel/units/UnitSystem.hpp"

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
        item->setLayer(layer);
        this->addChild(item);
    }
}

LayerTreeItem::LayerTreeItem(ViewModel* model)
    : TreeItem(model, "", QIcon(":/icons/model-layer.svg"), TreeItemType::LAYER)
{
    table = new TableEditor("Length", "Height", Quantities::ratio, Quantities::length, DoubleRange::nonNegative(1e-4), DoubleRange::positive(1e-4));
    combo = new QComboBox();
    updateCombo();

    setPlot(new SplineView("Length", "Height", Quantities::ratio, Quantities::length));
    updatePlot();

    QObject::connect(combo, &QComboBox::currentTextChanged, [=]{ updateModel(parent()); });
    QObject::connect(table, &TableEditor::modified, [=]{
        model->modifyLayer(row(), getLayer(), parent());
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

Layer LayerTreeItem::getLayer() const {
    return {
        .name = this->text(0).toStdString(),
        .material = combo->currentIndex(),
        .height = table->getData()
    };
}

void LayerTreeItem::setLayer(const Layer& layer) {
    this->setText(0, QString::fromStdString(layer.name));
    table->setData(layer.height);
    combo->setCurrentIndex(layer.material);
}

void LayerTreeItem::setData(int column, int role, const QVariant &value) {
    QTreeWidgetItem::setData(column, role, value);
    if(role == Qt::EditRole) {
        model->modifyLayer(row(), getLayer(), parent());
    }
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
