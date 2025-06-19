#pragma once
#include "pre/models/PropertyListModel.hpp"

class LineModel: public PropertyListModel {
public:
    QPersistentModelIndex LENGTH;

    LineModel(MainModel *parent, Line& line);
};
