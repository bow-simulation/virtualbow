#pragma once
#include "pre/viewmodels/PropertiesVM.hpp"

class MaterialVM: public PropertiesVM {
public:
    QPersistentModelIndex NAME;
    QPersistentModelIndex COLOR;
    QPersistentModelIndex DENSITY;
    QPersistentModelIndex YOUNGS_MODULUS;
    QPersistentModelIndex SHEAR_MODULUS;

    MaterialVM(MainVM *parent, Material& material);
};
