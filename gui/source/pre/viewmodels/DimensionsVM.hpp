#pragma once
#include "pre/viewmodels/PropertiesVM.hpp"

class DimensionsVM: public PropertiesVM {
public:
    QPersistentModelIndex BRACE_HEIGHT;
    QPersistentModelIndex DRAW_LENGTH;
    QPersistentModelIndex HANDLE_ORIGIN;
    QPersistentModelIndex HANDLE_LENGTH;
    QPersistentModelIndex HANDLE_OFFSET;
    QPersistentModelIndex HANDLE_ANGLE;

    DimensionsVM(MainVM *parent, Dimensions& dimensions);
};
