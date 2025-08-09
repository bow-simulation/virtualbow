#pragma once
#include "pre/models/PropertyListModel.hpp"

struct Arc;

class ArcModel: public PropertyListModel {
public:
    QPersistentModelIndex LENGTH;
    QPersistentModelIndex RADIUS;

    ArcModel(Arc& arc);
};
