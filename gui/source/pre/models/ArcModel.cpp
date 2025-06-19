#include "ArcModel.hpp"

ArcModel::ArcModel(MainModel *parent, Arc& arc):
    PropertyListModel(parent)
{
    LENGTH = addDouble(arc.length);
    RADIUS = addDouble(arc.radius);
}
