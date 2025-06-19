#pragma once
#include "pre/models/PropertyListModel.hpp"

class DampingModel: public PropertyListModel {
public:
    QPersistentModelIndex DAMPING_RATIO_LIMBS;
    QPersistentModelIndex DAMPING_RATIO_STRING;

    DampingModel(MainModel *parent, Damping& damping);
};
