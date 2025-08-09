#include "LineModel.hpp"
#include "solver/BowModel.hpp"

LineModel::LineModel(Line& line) {
    LENGTH = addDouble(line.length);
}
