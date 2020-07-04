#pragma once
#include "solver/numerics/Eigen.hpp"
#include <array>
#include <vector>

struct Point
{
    double s;
    double phi;
    double x;
    double y;
};

class Segment
{
public:
    // p: Starting point of the segment
    // c: Curve parameters for the segment
    Segment(const Point& p, const std::vector<double>& c);

    double phi(double s) const;
    double x(double s) const;
    double y(double s) const;
    double l() const;
    double I() const;

    static std::vector<double> get_c_start(const Point& p0, const Point& p1);

private:
    Point p;
    std::vector<double> c;

    static std::vector<double> get_c_start_x_y(const Point& p0, double x1, double y1);
    static std::vector<double> get_c_start_s_phi(const Point& p0, double s1, double phi1);
    static std::vector<double> get_c_start_s_x(const Point& p0, double s1, double x1);
    static std::vector<double> get_c_start_s_y(const Point& p0, double s1, double y1);
    static std::vector<double> get_c_start_phi_x(const Point& p0, double phi1, double x1);
    static std::vector<double> get_c_start_phi_y(const Point& p0, double phi1, double y1);
};

class ProfileCurve
{
public:
    ProfileCurve(const MatrixXd& input);
    const std::vector<Point>& get_points() const;

    // Arc length at start and end of the curve
    double s_min() const;
    double s_max() const;

    // Evaluate curve at a specific arc length. Returns {x, y, phi}.
    Vector<3> operator()(double s) const;

private:
    std::vector<Segment> segments;
    std::vector<Point> points;
    mutable size_t index;
};
