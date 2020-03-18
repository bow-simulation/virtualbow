#pragma once
#include "bow/LimbProperties.hpp"
#include <nlohmann/json.hpp>

using nlohmann::json;

struct SetupData
{
	LimbProperties limb_properties;
	double limb_mass = 0.0;
	double string_mass = 0.0;
	double string_length = 0.0;
};

static void to_json(json& obj, const SetupData& val)
{
	obj["limb_properties"] = val.limb_properties;
	obj["limb_mass"] = val.limb_mass;
	obj["string_mass"] = val.string_mass;
	obj["string_length"] = val.string_length;
}