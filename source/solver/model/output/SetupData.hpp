#pragma once
#include "solver/model/LimbProperties.hpp"
#include <nlohmann/json.hpp>

struct SetupData
{
	LimbProperties limb_properties;
	double limb_mass = 0.0;
	double string_mass = 0.0;
	double string_length = 0.0;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SetupData, limb_properties, limb_mass, string_mass, string_length)
