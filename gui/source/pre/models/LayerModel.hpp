#pragma once
#include "pre/models/PropertyListModel.hpp"

class LayerModel: public PropertyListModel {
public:
    QPersistentModelIndex NAME;
    QPersistentModelIndex MATERIAL;

    LayerModel(MainModel *parent, Layer& layer, const std::vector<Material>& materials);
    const QStringList& materialOptions() const;

private:
    QStringList materials;
};
