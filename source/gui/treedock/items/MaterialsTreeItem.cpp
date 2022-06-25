#include "MaterialsTreeItem.hpp"
#include "gui/viewmodel/ViewModel.hpp"
#include "gui/widgets/propertytree/PropertyTreeWidget.hpp"
#include "gui/widgets/propertytree/items/StringPropertyItem.hpp"
#include "gui/widgets/propertytree/items/ColorPropertyItem.hpp"
#include "gui/widgets/propertytree/items/DoublePropertyItem.hpp"

MaterialsTreeItem::MaterialsTreeItem(ViewModel* model)
    : TreeItem(model, "Materials", QIcon(":/icons/model-materials.svg"), TreeItemType::MATERIALS)
{
    updateView(nullptr);
    QObject::connect(model, &ViewModel::materialsModified, [=](void* source){
        updateView(source);
    });
}

void MaterialsTreeItem::updateModel(void* source) {
    std::vector<Material> materials;
    for(int i = 0; i < this->childCount(); ++i) {
        auto item = dynamic_cast<MaterialTreeItem*>(this->child(i));
        if(item != nullptr) {
            materials.push_back(item->getMaterial());
        }
    }
    model->setMaterials(materials, source);
}

void MaterialsTreeItem::updateView(void* source) {
    if(source != this) {
        // Remove children without notifying the model
        while(this->childCount() > 0) {
            QTreeWidgetItem::removeChild(this->child(0));
        }

        // Add new children
        for(auto& material: model->getMaterials()) {
            auto item = new MaterialTreeItem(model, material);
            this->addChild(item);
        }
    }
}

MaterialTreeItem::MaterialTreeItem(ViewModel* model, const Material& material)
    : TreeItem(model, QString::fromStdString(material.name), QIcon(":/icons/model-material.svg"), TreeItemType::MATERIAL)
{
    color = new ColorPropertyItem("Color");
    color->setValue(QString::fromStdString(material.color));

    rho = new DoublePropertyItem("Rho", Quantities::density, DoubleRange::positive(1.0));
    rho->setValue(material.rho);

    E = new DoublePropertyItem("E", Quantities::elastic_modulus, DoubleRange::positive(1e8));
    E->setValue(material.E);

    auto tree = new PropertyTreeWidget();
    tree->addTopLevelItem(color);
    tree->addTopLevelItem(rho);
    tree->addTopLevelItem(E);
    tree->expandAll();

    this->setEditor(tree);
    this->setFlags(this->flags() | Qt::ItemIsEditable);

    // Update view model on changes
    QObject::connect(tree, &QTreeWidget::itemChanged, [=]{
        updateModel(parent());
    });
}

Material MaterialTreeItem::getMaterial() const {
    return {
        .name = this->text(0).toStdString(),
        .color = color->getValue().name().toStdString(),
        .rho = rho->getValue(),
        .E = E->getValue()
    };
}

void MaterialTreeItem::setData(int column, int role, const QVariant &value) {
    QTreeWidgetItem::setData(column, role, value);
    updateModel(parent());
}
