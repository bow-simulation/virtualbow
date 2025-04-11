#include "DampingVM.hpp"

DampingVM::DampingVM(MainVM *parent, Damping& damping):
    PropertiesVM(parent)
{
    DAMPING_RATIO_LIMBS = addDouble(damping.damping_ratio_limbs);
    DAMPING_RATIO_STRING = addDouble(damping.damping_ratio_string);
}
