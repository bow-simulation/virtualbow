#pragma once
#include "pre/models/PropertyListModel.hpp"

class MaterialModel: public PropertyListModel {
public:
    QPersistentModelIndex NAME;
    QPersistentModelIndex COLOR;
    QPersistentModelIndex DENSITY;
    QPersistentModelIndex YOUNGS_MODULUS;
    QPersistentModelIndex SHEAR_MODULUS;

    MaterialModel(MainModel *parent, Material& material);
};
