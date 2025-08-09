#pragma once
#include "pre/models/PropertyListModel.hpp"

struct Masses;

class MassesModel: public PropertyListModel {
public:
    QPersistentModelIndex ARROW;
    QPersistentModelIndex STRING_CENTER;
    QPersistentModelIndex STRING_TIP;
    QPersistentModelIndex LIMB_TIP;

    MassesModel(Masses& masses);
};
