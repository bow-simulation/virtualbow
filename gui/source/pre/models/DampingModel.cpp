#include "DampingModel.hpp"
#include "solver/BowModel.hpp"

DampingModel::DampingModel(Damping& damping) {
    DAMPING_RATIO_LIMBS = addDouble(damping.damping_ratio_limbs);
    DAMPING_RATIO_STRING = addDouble(damping.damping_ratio_string);
}
