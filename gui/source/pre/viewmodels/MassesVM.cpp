#include "MassesVM.hpp"

MassesVM::MassesVM(MainVM *parent, Masses& masses):
    PropertiesVM(parent)
{
    ARROW = addDouble(masses.arrow);
    STRING_CENTER = addDouble(masses.string_center);
    STRING_TIP = addDouble(masses.string_tip);
    LIMB_TIP = addDouble(masses.limb_tip);
}
