#pragma once
#include "pre/models/PropertyListModel.hpp"

class SpiralModel: public PropertyListModel {
public:
    QPersistentModelIndex LENGTH;
    QPersistentModelIndex RADIUS_START;
    QPersistentModelIndex RADIUS_END;

    SpiralModel(MainModel *parent, Spiral& spiral);
};
