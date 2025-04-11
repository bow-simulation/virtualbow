#pragma once
#include "pre/viewmodels/PropertiesVM.hpp"

class LayerVM: public PropertiesVM {
public:
    QPersistentModelIndex NAME;
    QPersistentModelIndex MATERIAL;

    LayerVM(MainVM *parent, Layer& layer, const std::vector<Material>& materials);
    const QStringList& materialOptions() const;

private:
    QStringList materials;
};
