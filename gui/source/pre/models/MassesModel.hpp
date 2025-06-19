#pragma once
#include "pre/models/PropertyListModel.hpp"

class MassesModel: public PropertyListModel {
public:
    QPersistentModelIndex ARROW;
    QPersistentModelIndex STRING_CENTER;
    QPersistentModelIndex STRING_TIP;
    QPersistentModelIndex LIMB_TIP;

    MassesModel(MainModel *parent, Masses& masses);
};
