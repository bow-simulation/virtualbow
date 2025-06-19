#pragma once
#include "pre/models/PropertyListModel.hpp"

class LayersModel: public PropertyListModel {
public:
    QPersistentModelIndex ALIGNMENT;

    // TODO: Replace profile with layers once alignment was moved to layers
    LayersModel(MainModel *parent, Profile& layers);
};
