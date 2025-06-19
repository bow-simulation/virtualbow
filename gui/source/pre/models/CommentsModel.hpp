#pragma once
#include "pre/models/PropertyListModel.hpp"

class CommentsModel: public PropertyListModel {
public:
    QModelIndex COMMENT;

    CommentsModel(MainModel *parent, std::string& comment):
        PropertyListModel(parent)
    {
        COMMENT = addString(comment);
    }
};
