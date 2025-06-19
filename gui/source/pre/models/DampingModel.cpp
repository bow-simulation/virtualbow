#include "DampingModel.hpp"

DampingModel::DampingModel(MainModel *parent, Damping& damping):
    PropertyListModel(parent)
{
    DAMPING_RATIO_LIMBS = addDouble(damping.damping_ratio_limbs);
    DAMPING_RATIO_STRING = addDouble(damping.damping_ratio_string);
}
