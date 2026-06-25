#include "MassesModel.hpp"
#include "solver/BowModel.hpp"

MassesModel::MassesModel(Masses& masses) {
    ARROW = addCustom(masses.arrow);
    STRING_CENTER = addDouble(masses.string_center);
    STRING_END = addDouble(masses.string_end);
    LIMB_TIP = addDouble(masses.limb_tip);
}
