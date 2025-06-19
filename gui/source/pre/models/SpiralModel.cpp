#include "SpiralModel.hpp"

SpiralModel::SpiralModel(MainModel *parent, Spiral& spiral):
    PropertyListModel(parent)
{
    LENGTH = addDouble(spiral.length);
    RADIUS_START = addDouble(spiral.radius_start);
    RADIUS_END = addDouble(spiral.radius_end);
}
