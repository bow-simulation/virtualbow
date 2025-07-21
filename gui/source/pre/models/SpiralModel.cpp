#include "SpiralModel.hpp"
#include "solver/BowModel.hpp"

SpiralModel::SpiralModel(Spiral& spiral) {
    LENGTH = addDouble(spiral.length);
    RADIUS_START = addDouble(spiral.radius_start);
    RADIUS_END = addDouble(spiral.radius_end);
}
