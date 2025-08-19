#pragma once
#include "pre/models/PropertyListModel.hpp"

struct Material;

class MaterialModel: public PropertyListModel {
public:
    QPersistentModelIndex NAME;
    QPersistentModelIndex COLOR;
    QPersistentModelIndex DENSITY;
    QPersistentModelIndex YOUNGS_MODULUS;
    QPersistentModelIndex SHEAR_MODULUS;
    QPersistentModelIndex TENSILE_STRENGTH;
    QPersistentModelIndex COMPRESSIVE_STRENGTH;
    QPersistentModelIndex SAFETY_MARGIN;

    MaterialModel(Material& material);
};
