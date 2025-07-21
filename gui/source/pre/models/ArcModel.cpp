#include "ArcModel.hpp"
#include "solver/BowModel.hpp"

ArcModel::ArcModel(Arc& arc) {
    LENGTH = addDouble(arc.length);
    RADIUS = addDouble(arc.radius);
}
