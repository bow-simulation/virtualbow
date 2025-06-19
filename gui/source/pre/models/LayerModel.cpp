#include "LayerModel.hpp"

LayerModel::LayerModel(MainModel *parent, Layer& layer, const std::vector<Material>& materials):
    PropertyListModel(parent)
{
    NAME = addString(layer.name);
    MATERIAL = addString(layer.material);

    for(const Material& material: materials) {
        this->materials.append(QString::fromStdString(material.name));
    }
}

const QStringList& LayerModel::materialOptions() const {
    return materials;
}
