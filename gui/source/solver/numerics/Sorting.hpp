#pragma once
#include "EigenTypes.hpp"
#include <vector>

// Sorts the vector args in ascending order and applies the same permutation to vals
// Todo: Remove in favor of version below
void sort_by_argument(std::vector<double>& args, std::vector<double>& vals);
void sort_by_argument(VectorXd& args, VectorXd& vals);
