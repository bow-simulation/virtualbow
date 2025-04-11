#pragma once
#include "pre/viewmodels/PropertiesVM.hpp"

class LineVM: public PropertiesVM {
public:
    QPersistentModelIndex LENGTH;

    LineVM(MainVM *parent, Line& line);
};
