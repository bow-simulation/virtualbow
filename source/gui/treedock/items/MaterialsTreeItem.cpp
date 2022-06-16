#include "MaterialsTreeItem.hpp"
#include "gui/viewmodel/DataViewModel.hpp"
#include "gui/widgets/propertytree/PropertyTreeWidget.hpp"
#include "gui/widgets/propertytree/items/StringPropertyItem.hpp"
#include "gui/widgets/propertytree/items/ColorPropertyItem.hpp"
#include "gui/widgets/propertytree/items/DoublePropertyItem.hpp"

MaterialsTreeItem::MaterialsTreeItem(DataViewModel* model)
    : TreeItem("Materials", QIcon(":/icons/model-materials.svg"), TreeItemType::MATERIALS),
      model(model)
{
    updateView();
}

void MaterialsTreeItem::updateModel() {
    std::vector<Material> materials;
    for(int i = 0; i < this->childCount(); ++i) {
        auto item = dynamic_cast<MaterialTreeItem*>(this->child(i));
        if(item != nullptr) {
            materials.push_back(item->getMaterial());
        }
    }
    model->setMaterials(materials);
}

void MaterialsTreeItem::updateView() {
    for(auto& material: model->getMaterials()) {
        auto item = new MaterialTreeItem(model, material);
        this->addChild(item);
    }
}

void MaterialsTreeItem::insertChild(int i, QTreeWidgetItem* item) {
    TreeItem::insertChild(i, item);
    updateModel();
}

void MaterialsTreeItem::removeChild(int i) {
    TreeItem::removeChild(i);
    updateModel();
}

void MaterialsTreeItem::swapChildren(int i, int j) {
    TreeItem::swapChildren(i, j);
    updateModel();
}

MaterialTreeItem::MaterialTreeItem(DataViewModel* model, const Material& material)
    : TreeItem(QString::fromStdString(material.name), QIcon(":/icons/model-material.svg"), TreeItemType::MATERIAL)
{
    color = new ColorPropertyItem("Color");
    color->setValue(QString::fromStdString(material.color));

    rho = new DoublePropertyItem("Rho", &UnitSystem::density, DoubleRange::positive(1.0));
    rho->setValue(material.rho);

    E = new DoublePropertyItem("E", &UnitSystem::elastic_modulus, DoubleRange::positive(1e8));
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
        updateViewModel();
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
    updateViewModel();
}

void MaterialTreeItem::updateViewModel() {
    auto parent = dynamic_cast<MaterialsTreeItem*>(this->parent());
    if(parent != nullptr) {
        parent->updateModel();
    }
}
