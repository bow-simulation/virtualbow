#include "MassesModel.hpp"

MassesModel::MassesModel(MainModel *parent, Masses& masses):
    PropertyListModel(parent)
{
    ARROW = addDouble(masses.arrow);
    STRING_CENTER = addDouble(masses.string_center);
    STRING_TIP = addDouble(masses.string_tip);
    LIMB_TIP = addDouble(masses.limb_tip);
}
