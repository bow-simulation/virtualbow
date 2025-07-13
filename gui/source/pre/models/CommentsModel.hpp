#pragma once
#include "pre/models/PropertyListModel.hpp"

class CommentsModel: public PropertyListModel {
public:
    QPersistentModelIndex COMMENT;

    CommentsModel(MainModel *parent, std::string& comment);
};
