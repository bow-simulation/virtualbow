#include "StringModel.hpp"
#include "solver/BowModel.hpp"

StringModel::StringModel(BowString& string) {
    STRAND_STIFFNESS = addDouble(string.strand_stiffness);
    STRAND_DENSITY = addDouble(string.strand_density);
    N_STRANDS = addInteger(string.num_strands);
}
