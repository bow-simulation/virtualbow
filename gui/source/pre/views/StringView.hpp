#pragma once
#include "pre/widgets/PropertyList.hpp"

class StringModel;

class StringView: public PropertyList
{
public:
    StringView(StringModel* model);
};
