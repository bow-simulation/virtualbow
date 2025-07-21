#pragma once
#include "pre/models/PropertyListModel.hpp"

struct Profile;

class LayersModel: public PropertyListModel {
public:
    QPersistentModelIndex ALIGNMENT;

    // TODO: Replace profile with layers once alignment was moved to layers
    LayersModel(Profile& layers);
};
