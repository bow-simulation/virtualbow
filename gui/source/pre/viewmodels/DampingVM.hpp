#pragma once
#include "pre/viewmodels/PropertiesVM.hpp"

class DampingVM: public PropertiesVM {
public:
    QPersistentModelIndex DAMPING_RATIO_LIMBS;
    QPersistentModelIndex DAMPING_RATIO_STRING;

    DampingVM(MainVM *parent, Damping& damping);
};
