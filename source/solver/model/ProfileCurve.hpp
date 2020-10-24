#pragma once
#include <optional>
#include <vector>
#include <cmath>


#include <nlopt.hpp>
#include "solver/numerics/LinearSpline.hpp"
#include "solver/numerics/Integration.hpp"
#include "solver/numerics/Linspace.hpp"
#include "solver/numerics/Eigen.hpp"

struct Point {
    double x;
    double y;
    double phi;
};

struct SegmentInput {
    std::optional<double> length;
    std::optional<double> angle;
    std::optional<double> delta_x;
    std::optional<double> delta_y;
};

class Segment {
public:
    Segment(const std::vector<double>& c);

    static std::vector<double> estimate_coeffs(const Point& point, const SegmentInput& input);
    const std::vector<double>& get_coeffs() const;
    void set_coeffs(const std::vector<double>&  c);

    double length() const;
    double angle() const;
    double energy() const;

    double phi(double t) const;    // Orientation angle in [-PI, PI]
    double x(double t) const;
    double y(double t) const;

    double dxdt(double t) const;
    double dydt(double t) const;

    double dxdt2(double t) const;
    double dydt2(double t) const;

private:
    std::vector<double> c;
};

class ProfileCurve {
public:
    static ProfileCurve from_matrix(const MatrixXd& matrix);
    ProfileCurve(const std::vector<SegmentInput>& input);

    const std::vector<Segment>& get_segments() const;
    const std::vector<Point>& get_nodes() const;
    double length() const;
    Point operator()(double s) const;

private:
    std::vector<Segment> segments;
    std::vector<Point> points;
    LinearSpline transform;

    static Segment optimize_local(const Point& point, const SegmentInput& input);
};
