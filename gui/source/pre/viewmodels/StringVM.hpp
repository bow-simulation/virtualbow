#pragma once
#include "pre/viewmodels/PropertiesVM.hpp"

class StringVM: public PropertiesVM {
public:
    QPersistentModelIndex STRAND_STIFFNESS;
    QPersistentModelIndex STRAND_DENSITY;
    QPersistentModelIndex N_STRANDS;

    StringVM(MainVM *parent, String& string);
};
