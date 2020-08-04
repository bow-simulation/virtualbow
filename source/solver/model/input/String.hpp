#pragma once
#include <nlohmann/json.hpp>

struct String
{
    double strand_stiffness = 3500.0;
    double strand_density = 0.0005;
    int n_strands = 12;
};

static bool operator==(const String& lhs, const String& rhs)
{
    return lhs.strand_stiffness == rhs.strand_stiffness
        && lhs.strand_density == rhs.strand_density
        && lhs.n_strands == rhs.n_strands;
}

static bool operator!=(const String& lhs, const String& rhs)
{
    return !operator==(lhs, rhs);
}

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(String, strand_stiffness, strand_density, n_strands)
