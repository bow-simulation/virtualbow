#pragma once
#include "pre/models/PropertyListModel.hpp"

struct Masses;

class MassesModel: public PropertyListModel {
public:
    QPersistentModelIndex ARROW;
    QPersistentModelIndex STRING_CENTER;
    QPersistentModelIndex STRING_END;
    QPersistentModelIndex LIMB_TIP;

    MassesModel(Masses& masses);
};
