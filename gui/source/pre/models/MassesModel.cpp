#include "MassesModel.hpp"
#include "solver/BowModel.hpp"

MassesModel::MassesModel(Masses& masses) {
    ARROW = addArrowMass(masses.arrow);
    STRING_CENTER = addDouble(masses.string_center);
    STRING_TIP = addDouble(masses.string_tip);
    LIMB_TIP = addDouble(masses.limb_tip);
}
