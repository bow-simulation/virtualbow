#include "LineVM.hpp"

LineVM::LineVM(MainVM *parent, Line& line):
    PropertiesVM(parent)
{
    LENGTH = addDouble(line.length);
}
