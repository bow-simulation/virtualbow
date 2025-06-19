#pragma once
#include "pre/models/PropertyListModel.hpp"

class StringModel: public PropertyListModel {
public:
    QPersistentModelIndex STRAND_STIFFNESS;
    QPersistentModelIndex STRAND_DENSITY;
    QPersistentModelIndex N_STRANDS;

    StringModel(MainModel *parent, String& string);
};
