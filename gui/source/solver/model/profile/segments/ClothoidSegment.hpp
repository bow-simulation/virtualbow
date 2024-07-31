#pragma once
#include "solver/model/profile/ProfileSegment.hpp"
#include <nlohmann/json.hpp>
#include <unordered_map>

enum class SpiralConstraint{ LENGTH, R_START, R_END };
enum class ArcConstraint{ LENGTH, RADIUS };
enum class LineConstraint{ LENGTH };

using SpiralInput = std::map<SpiralConstraint, double>;
using ArcInput = std::map<ArcConstraint, double>;
using LineInput = std::map<LineConstraint, double>;

// Nlohmann::json requires map types to have keys that are convertible to strings.
// This is not true in this case, so the (de)serialization is defined manually.

void to_json(nlohmann::json& obj, const SpiralInput& input);
void from_json(const nlohmann::json& obj, SpiralInput& input);

void to_json(nlohmann::json& obj, const ArcInput& input);
void from_json(const nlohmann::json& obj, ArcInput& input);

void to_json(nlohmann::json& obj, const LineInput& input);
void from_json(const nlohmann::json& obj, LineInput& input);

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
