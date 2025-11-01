#pragma once
#include "pre/models/PropertyListModel.hpp"

struct Layer;
struct Material;

class LayerModel: public PropertyListModel {
public:
    QPersistentModelIndex NAME;
    QPersistentModelIndex MATERIAL;

    LayerModel(Layer& layer, const std::list<Material>& materials);
    const QStringList& materialOptions() const;

private:
    QStringList materials;
};
