#include "LayerModel.hpp"
#include "solver/BowModel.hpp"

LayerModel::LayerModel(Layer& layer, const std::list<Material>& materials) {
    NAME = addString(layer.name);
    MATERIAL = addString(layer.material);

    for(const Material& material: materials) {
        this->materials.append(QString::fromStdString(material.name));
    }
}

const QStringList& LayerModel::materialOptions() const {
    return materials;
}
