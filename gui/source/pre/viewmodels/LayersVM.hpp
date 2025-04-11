#pragma once
#include "pre/viewmodels/PropertiesVM.hpp"

class LayersVM: public PropertiesVM {
public:
    QPersistentModelIndex ALIGNMENT;

    // TODO: Replace profile with layers once alignment was moved to layers
    LayersVM(MainVM *parent, Profile& layers);
};
