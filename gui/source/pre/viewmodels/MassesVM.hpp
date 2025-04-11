#pragma once
#include "pre/viewmodels/PropertiesVM.hpp"

class MassesVM: public PropertiesVM {
public:
    QPersistentModelIndex ARROW;
    QPersistentModelIndex STRING_CENTER;
    QPersistentModelIndex STRING_TIP;
    QPersistentModelIndex LIMB_TIP;

    MassesVM(MainVM *parent, Masses& masses);
};
