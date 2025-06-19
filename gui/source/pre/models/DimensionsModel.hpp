#pragma once
#include "pre/models/PropertyListModel.hpp"

class DimensionsModel: public PropertyListModel {
public:
    QPersistentModelIndex BRACE_HEIGHT;
    QPersistentModelIndex DRAW_LENGTH;
    QPersistentModelIndex HANDLE_ORIGIN;
    QPersistentModelIndex HANDLE_LENGTH;
    QPersistentModelIndex HANDLE_OFFSET;
    QPersistentModelIndex HANDLE_ANGLE;

    DimensionsModel(MainModel *parent, Dimensions& dimensions);
};
