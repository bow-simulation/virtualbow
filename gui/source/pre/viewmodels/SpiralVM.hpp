#pragma once
#include "pre/viewmodels/PropertiesVM.hpp"

class SpiralVM: public PropertiesVM {
public:
    QPersistentModelIndex LENGTH;
    QPersistentModelIndex RADIUS_START;
    QPersistentModelIndex RADIUS_END;

    SpiralVM(MainVM *parent, Spiral& spiral);
};
