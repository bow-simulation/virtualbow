#pragma once
#include "pre/models/PropertyListModel.hpp"

struct Line;

class LineModel: public PropertyListModel {
public:
    QPersistentModelIndex LENGTH;

    LineModel(Line& line);
};
