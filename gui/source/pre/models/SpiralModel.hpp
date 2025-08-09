#pragma once
#include "pre/models/PropertyListModel.hpp"

struct Spiral;

class SpiralModel: public PropertyListModel {
public:
    QPersistentModelIndex LENGTH;
    QPersistentModelIndex RADIUS_START;
    QPersistentModelIndex RADIUS_END;

    SpiralModel(Spiral& spiral);
};
