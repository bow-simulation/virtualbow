#include "Rounding.hpp"
#include <cmath>

double floorToPow10(double x) {
    if (x <= 0) {
        return 0.0;
    }

    return std::pow(10, std::floor(std::log10(x)));
}
