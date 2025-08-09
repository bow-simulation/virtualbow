#include "DimensionsModel.hpp"
#include "solver/BowModel.hpp"

DimensionsModel::DimensionsModel(Dimensions& dimensions) {
    BRACE_HEIGHT = addDouble(dimensions.brace_height);
    DRAW_LENGTH = addDouble(dimensions.draw_length);
    HANDLE_REFERENCE = addEnum(dimensions.handle_reference);
    HANDLE_LENGTH = addDouble(dimensions.handle_length);
    HANDLE_OFFSET = addDouble(dimensions.handle_offset);
    HANDLE_ANGLE = addDouble(dimensions.handle_angle);
}
