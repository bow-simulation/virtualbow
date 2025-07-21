#pragma once
#include "pre/models/PropertyListModel.hpp"

struct Damping;

class DampingModel: public PropertyListModel {
public:
    QPersistentModelIndex DAMPING_RATIO_LIMBS;
    QPersistentModelIndex DAMPING_RATIO_STRING;

    DampingModel(Damping& damping);
};
