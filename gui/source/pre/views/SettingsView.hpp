#pragma once
#include "pre/widgets/PropertyList.hpp"

class SettingsModel;

class SettingsView: public PropertyList
{
public:
    SettingsView(SettingsModel* model);
};
