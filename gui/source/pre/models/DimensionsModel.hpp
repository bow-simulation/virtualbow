#pragma once
#include "pre/models/PropertyListModel.hpp"

struct Dimensions;

class DimensionsModel: public PropertyListModel {
public:
    QPersistentModelIndex BRACE_HEIGHT;
    QPersistentModelIndex DRAW_LENGTH;
    QPersistentModelIndex HANDLE_REFERENCE;
    QPersistentModelIndex HANDLE_LENGTH;
    QPersistentModelIndex HANDLE_OFFSET;
    QPersistentModelIndex HANDLE_ANGLE;

    DimensionsModel(Dimensions& dimensions);
};
