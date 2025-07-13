#include "CommentsModel.hpp"

CommentsModel::CommentsModel(MainModel *parent, std::string& comment):
    PropertyListModel(parent)
{
    COMMENT = addString(comment);
}
