#include "ArcVM.hpp"

ArcVM::ArcVM(MainVM *parent, Arc& arc):
    PropertiesVM(parent)
{
    LENGTH = addDouble(arc.length);
    RADIUS = addDouble(arc.radius);
}
