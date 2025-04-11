#include "SpiralVM.hpp"

SpiralVM::SpiralVM(MainVM *parent, Spiral& spiral):
    PropertiesVM(parent)
{
    LENGTH = addDouble(spiral.length);
    RADIUS_START = addDouble(spiral.radius_start);
    RADIUS_END = addDouble(spiral.radius_end);
}
