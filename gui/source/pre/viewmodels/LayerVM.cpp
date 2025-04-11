#include "LayerVM.hpp"

LayerVM::LayerVM(MainVM *parent, Layer& layer, const std::vector<Material>& materials):
    PropertiesVM(parent)
{
    NAME = addString(layer.name);
    MATERIAL = addString(layer.material);

    for(const Material& material: materials) {
        this->materials.append(QString::fromStdString(material.name));
    }
}

const QStringList& LayerVM::materialOptions() const {
    return materials;
}
