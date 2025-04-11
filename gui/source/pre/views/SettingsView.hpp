#pragma once
#include "pre/widgets/PropertyList.hpp"

class SettingsVM;

class SettingsView: public PropertyList
{
public:
    SettingsView(SettingsVM* model);
};
