#pragma once
#include "pre/viewmodels/PropertiesVM.hpp"

class ArcVM: public PropertiesVM {
public:
    QPersistentModelIndex LENGTH;
    QPersistentModelIndex RADIUS;

    ArcVM(MainVM *parent, Arc& arc);
};
