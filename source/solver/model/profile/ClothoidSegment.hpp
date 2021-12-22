#pragma once
#include "ProfileSegment.hpp"
#include <nlohmann/json.hpp>
#include <unordered_map>

enum class SpiralConstraint{ LENGTH, K_START, K_END };
NLOHMANN_JSON_SERIALIZE_ENUM(SpiralConstraint, {{SpiralConstraint::LENGTH, "length"}, {SpiralConstraint::K_START, "k_start"}, {SpiralConstraint::K_END, "k_end"}})

enum class ArcConstraint{ LENGTH, K_START };
NLOHMANN_JSON_SERIALIZE_ENUM(ArcConstraint, {{ArcConstraint::LENGTH, "length"}, {ArcConstraint::K_START, "k_start"}})

enum class LineConstraint{ LENGTH };
NLOHMANN_JSON_SERIALIZE_ENUM(LineConstraint, {{LineConstraint::LENGTH, "length"}})

using SpiralInput = std::unordered_map<SpiralConstraint, double>;
using ArcInput = std::unordered_map<ArcConstraint, double>;
using LineInput = std::unordered_map<LineConstraint, double>;

// Segment with curvature linearly varying over arc length.
// Represents a clothoid, circular arc or a straight line depending on the choice of curvature.
class ClothoidSegment: public ProfileSegment {
public:
    ClothoidSegment(const Point& start, const SpiralInput& input);
    ClothoidSegment(const Point& start, const ArcInput& input);
    ClothoidSegment(const Point& start, const LineInput& input);
    ClothoidSegment(const Point& start, double l, double k0, double k1);

    double s_start() const override;
    double s_end() const override;

    double curvature(double s) const override;
    double angle(double s) const override;
    Vector<2> position(double s) const override;

private:
    double x0;
    double y0;

    double s0;
    double s1;

    double a;
    double b;
    double c;
};
