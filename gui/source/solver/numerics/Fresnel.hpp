#pragma once
#include <utility>

// Calculates the (normalized) fresnel integrals S(x) and C(x).
std::pair<double, double> fresnel(double x);
