#pragma once
#include "pre/models/PropertyListModel.hpp"

class HandleModel: public PropertyListModel {
public:
    QPersistentModelIndex HANDLE;

    HandleModel(Handle& handle);
};
