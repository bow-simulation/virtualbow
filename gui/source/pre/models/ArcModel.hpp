#pragma once
#include "pre/models/PropertyListModel.hpp"

class ArcModel: public PropertyListModel {
public:
    QPersistentModelIndex LENGTH;
    QPersistentModelIndex RADIUS;

    ArcModel(MainModel *parent, Arc& arc);
};
