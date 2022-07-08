#include "MaterialsTreeItem.hpp"
#include "gui/viewmodel/ViewModel.hpp"
#include "gui/widgets/propertytree/PropertyTreeWidget.hpp"
#include "gui/widgets/propertytree/items/StringPropertyItem.hpp"
#include "gui/widgets/propertytree/items/ColorPropertyItem.hpp"
#include "gui/widgets/propertytree/items/DoublePropertyItem.hpp"

MaterialsTreeItem::MaterialsTreeItem(ViewModel* model)
    : TreeItem(model, "Materials", QIcon(":/icons/model-materials.svg"), TreeItemType::MATERIALS)
{
    QObject::connect(model, &ViewModel::reloaded, [=]{
        initFromModel();
    });

    QObject::connect(model, &ViewModel::materialModified, [=](int i, void* source) {
        if(source != this) {
            auto item = dynamic_cast<MaterialTreeItem*>(this->child(i));
            item->setMaterial(model->getData().materials[i]);
        }
    });

    QObject::connect(model, &ViewModel::materialInserted, [=](int i, void* source) {
        if(source != this) {
            auto item = new MaterialTreeItem(model);
            item->setMaterial(model->getData().materials[i]);
            this->insertChild(i, item);
        }
    });

    QObject::connect(model, &ViewModel::materialRemoved, [=](int i, void* source) {
        if(source != this) {
            this->removeChild(i);
        }
    });

    QObject::connect(model, &ViewModel::materialsSwapped, [=](int i, int j, void* source) {
        if(source != this) {
            this->swapChildren(i, j);
        }
    });

    initFromModel();
}

void MaterialsTreeItem::initFromModel() {
    this->removeChildren();
    for(auto& material: model->getMaterials()) {
        auto item = new MaterialTreeItem(model);
        item->setMaterial(material);
        this->addChild(item);
    }
}

MaterialTreeItem::MaterialTreeItem(ViewModel* model)
    : TreeItem(model, "", QIcon(":/icons/model-material.svg"), TreeItemType::MATERIAL)
{
    color = new ColorPropertyItem("Color");
    rho = new DoublePropertyItem("Rho", Quantities::density, DoubleRange::positive(1.0));
    E = new DoublePropertyItem("E", Quantities::elastic_modulus, DoubleRange::positive(1e8));

    auto tree = new PropertyTreeWidget();
    tree->addTopLevelItem(color);
    tree->addTopLevelItem(rho);
    tree->addTopLevelItem(E);
    tree->expandAll();

    this->setEditor(tree);
    this->setFlags(this->flags() | Qt::ItemIsEditable);

    // Update viewmodel on changes
    QObject::connect(tree, &QTreeWidget::itemChanged, [=]{
        model->modifyMaterial(row(), getMaterial(), parent());
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

void MaterialTreeItem::setMaterial(const Material& material) {
    QSignalBlocker blocker(editor);

    this->setText(0, QString::fromStdString(material.name));
    color->setValue(QString::fromStdString(material.color));
    rho->setValue(material.rho);
    E->setValue(material.E);
}

void MaterialTreeItem::setData(int column, int role, const QVariant &value) {
    QTreeWidgetItem::setData(column, role, value);
    if(role == Qt::EditRole) {
        model->modifyMaterial(row(), getMaterial(), parent());
    }
}
