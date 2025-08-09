#pragma once
#include "pre/widgets/PropertyList.hpp"

class LineModel;

class LineView: public PropertyList
{
public:
    LineView(LineModel* model);
};
