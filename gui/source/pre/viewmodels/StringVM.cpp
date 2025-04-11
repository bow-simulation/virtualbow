#include "StringVM.hpp"

StringVM::StringVM(MainVM *parent, String& string):
    PropertiesVM(parent)
{
    STRAND_STIFFNESS = addDouble(string.strand_stiffness);
    STRAND_DENSITY = addDouble(string.strand_density);
    N_STRANDS = addInteger(string.n_strands);
}
