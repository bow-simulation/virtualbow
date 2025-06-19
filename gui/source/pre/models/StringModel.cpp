#include "StringModel.hpp"

StringModel::StringModel(MainModel *parent, String& string):
    PropertyListModel(parent)
{
    STRAND_STIFFNESS = addDouble(string.strand_stiffness);
    STRAND_DENSITY = addDouble(string.strand_density);
    N_STRANDS = addInteger(string.n_strands);
}
