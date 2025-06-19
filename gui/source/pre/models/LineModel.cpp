#include "LineModel.hpp"

LineModel::LineModel(MainModel *parent, Line& line):
    PropertyListModel(parent)
{
    LENGTH = addDouble(line.length);
}
